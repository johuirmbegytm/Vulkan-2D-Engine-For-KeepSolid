#pragma once

#include "vefp_window.hpp"
#include "vefp_device.hpp"
#include "vefp_swap_chain.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace vefp {
	class VefpRenderer {
	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		VefpWindow& vefpWindow;
		VefpDevice& vefpDevice;
		std::unique_ptr<VefpSwapChain> vefpSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;

	public:

		VefpRenderer(VefpWindow &window, VefpDevice &device);
		~VefpRenderer();

		VefpRenderer(const VefpRenderer&) = delete;
		VefpRenderer& operator=(const VefpRenderer&) = delete;
	
		VkRenderPass getSwapChainRenderPass() const { return vefpSwapChain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	
	};

}