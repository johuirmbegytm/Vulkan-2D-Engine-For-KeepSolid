#pragma once

#include "vefp_window.hpp"
#include "vefp_device.hpp"
#include "vefp_swap_chain.hpp"
#include "vefp_app_object.hpp"
#include "vefp_renderer.hpp"

#include <memory>
#include <vector>

namespace vefp {
	class FirstApp {
	 private:
		void loadAppObjects();
		
		VefpWindow vefpWindow{WIDTH, HEIGHT, "Vulkan Engine For Practice"};
		VefpDevice vefpDevice{ vefpWindow };
		VefpRenderer vefpRenderer{ vefpWindow, vefpDevice };

		std::vector<VefpAppObject> appObjects;

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		std::unique_ptr<VefpModel> createSquareModel(VefpDevice& device, glm::vec2 offset);
		std::unique_ptr<VefpModel> createCircleModel(VefpDevice& device, unsigned int numSides);

		void run();
	};

}