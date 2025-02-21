#pragma once
 
#include "vefp_device.hpp"
#include "vefp_pipeline.hpp"
#include "vefp_app_object.hpp"


#include <memory>
#include <vector>

namespace vefp {
	class SimpleRenderSystem {
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		VefpDevice& vefpDevice;

		std::unique_ptr<VefpPipeline> vefpPipeline;
		VkPipelineLayout pipelineLayout;

	public:

		SimpleRenderSystem(VefpDevice& device, VkRenderPass renderpass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderAppObjects(VkCommandBuffer commandBuffer, std::vector<VefpAppObject> &appObjects);

	};

}