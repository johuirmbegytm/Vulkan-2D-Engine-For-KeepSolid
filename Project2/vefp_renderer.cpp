#include "vefp_renderer.hpp"


#include <stdexcept>
#include <array>

namespace vefp {


	VefpRenderer::VefpRenderer(VefpWindow& window, VefpDevice& device) : vefpWindow{ window }, vefpDevice{ device } {
		recreateSwapChain();
		createCommandBuffers();
	}

	VefpRenderer::~VefpRenderer() {
		freeCommandBuffers();
	}

	void VefpRenderer::recreateSwapChain() {
		auto extent = vefpWindow.getExtend();
		while (extent.width == 0 || extent.height == 0) {
			extent = vefpWindow.getExtend();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(vefpDevice.device());

		if (vefpSwapChain == nullptr) {
			vefpSwapChain = std::make_unique<VefpSwapChain>(vefpDevice, extent);
		}
		else {
			std::shared_ptr<VefpSwapChain> oldSwapChain = std::move(vefpSwapChain);
			vefpSwapChain = std::make_unique<VefpSwapChain>(vefpDevice, extent, std::move(vefpSwapChain));
			
			if (!oldSwapChain->compareSwapFormats(*vefpSwapChain.get())) {
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}
		

		// we'll come back to this in just a moment
	}

	void VefpRenderer::createCommandBuffers() {
		commandBuffers.resize(VefpSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vefpDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(vefpDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void VefpRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(
			vefpDevice.device(),
			vefpDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer VefpRenderer::beginFrame() {
		assert(!isFrameStarted && "Can't call beginFrame while already in progress");
		auto result = vefpSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		return commandBuffer;
	}

	void VefpRenderer::endFrame() {
		assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
		auto result = vefpSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vefpWindow.wasWindowResized()) {
			vefpWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % VefpSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void VefpRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(
			commandBuffer == getCurrentCommandBuffer() &&
			"Can't begin render pass on command buffer from a different frame");
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vefpSwapChain->getRenderPass();
		renderPassInfo.framebuffer = vefpSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = vefpSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(vefpSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(vefpSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor({ 0,0 }, vefpSwapChain->getSwapChainExtent());
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void VefpRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(
			commandBuffer == getCurrentCommandBuffer() &&
			"Can't end render pass on command buffer from a different frame");
		vkCmdEndRenderPass(commandBuffer);

	}

}