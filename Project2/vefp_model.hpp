#pragma once

#include "vefp_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vefp {
	class VefpModel {
	 public:

		 struct Vertex {
			 glm::vec2 position;
			 glm::vec3 color;

			 static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			 static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		 };

		 VefpModel(VefpDevice &device, const std::vector<Vertex>& vertices);
		 ~VefpModel();

		 VefpModel(const VefpModel&) = delete;
		 VefpModel& operator=(const VefpModel&) = delete;

		 void bind(VkCommandBuffer commandBuffer);
		 void draw(VkCommandBuffer commandBuffer);

	 private:

		 void createVertexBuffers(const std::vector<Vertex> &vertices);

		 VefpDevice& vefpDevice;
		 VkBuffer vertexBuffer;
		 VkDeviceMemory vertexBufferMemory;
		 uint32_t vertexCount;
	};
}