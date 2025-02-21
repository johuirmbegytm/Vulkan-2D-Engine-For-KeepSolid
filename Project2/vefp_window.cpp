#include "vefp_window.hpp"

#include <stdexcept>

namespace vefp {

	VefpWindow::VefpWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	VefpWindow::~VefpWindow() {
		glfwDestroyWindow(window);
		glfwTerminate(); 
	}

	void VefpWindow::initWindow(){
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
	}
	void VefpWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}
	}

	void VefpWindow::frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto vefpWindow = reinterpret_cast<VefpWindow*>(glfwGetWindowUserPointer(window));
		vefpWindow->frameBufferResized = true;
		vefpWindow->width = width;
		vefpWindow->height = height;
	}
}