#include "first_app.hpp"
#include "physics_and_field.hpp"

#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace vefp {

	std::unique_ptr<VefpModel> FirstApp::createSquareModel(VefpDevice& device, glm::vec2 offset) {
		std::vector<VefpModel::Vertex> vertices = {
			{{-0.5f, -0.5f}},
			{{0.5f, 0.5f}},
			{{-0.5f, 0.5f}},
			{{-0.5f, -0.5f}},
			{{0.5f, -0.5f}},
			{{0.5f, 0.5f}},
		};
		for (auto& v : vertices) {
			v.position += offset;
		}

		return std::make_unique<VefpModel>(device, vertices);
	}

	std::unique_ptr<VefpModel> FirstApp::createCircleModel(VefpDevice& device, unsigned int numSides) {
		std::vector<VefpModel::Vertex> uniqueVertices{};
		for (int i = 0; i < numSides; i++) {
			float angle = i * glm::two_pi<float>() / numSides;
			uniqueVertices.push_back({ {glm::cos(angle), glm::sin(angle)} });
		}
		uniqueVertices.push_back({});  // adds center vertex at 0, 0

		std::vector<VefpModel::Vertex> vertices{};
		for (int i = 0; i < numSides; i++) {
			vertices.push_back(uniqueVertices[i]);
			vertices.push_back(uniqueVertices[(i + 1) % numSides]);
			vertices.push_back(uniqueVertices[numSides]);
		}
		return std::make_unique<VefpModel>(device, vertices);
	}

	FirstApp::FirstApp() {
		loadAppObjects();
	}
	
	FirstApp::~FirstApp() {

	}


	void FirstApp::run() {

		// create some models
		std::shared_ptr<VefpModel> squareModel = createSquareModel(
			vefpDevice,
			{ .7f, .0f });  // offset model by .5 so rotation occurs at edge rather than center of square
		std::shared_ptr<VefpModel> circleModel = createCircleModel(vefpDevice, 64);

		// create physics objects
		std::vector<VefpAppObject> physicsObjects{};
		auto yellow = VefpAppObject::createAppObject();
		yellow.transform2d.scale = glm::vec2{ .05f };
		yellow.transform2d.translation = { .5f, .5f };
		yellow.color = { .8f, 0.5f, 0.f };
		yellow.rigidBody2d.velocity = { -.5f, .0f };
		yellow.model = circleModel;
		physicsObjects.push_back(std::move(yellow));
		auto blue = VefpAppObject::createAppObject();
		blue.transform2d.scale = glm::vec2{ .05f };
		blue.transform2d.translation = { -.45f, -.25f };
		blue.color = { 0.f, 0.1f, 0.9f };
		blue.rigidBody2d.velocity = { .5f, .0f };
		blue.model = circleModel;
		physicsObjects.push_back(std::move(blue));

		// create vector field
		std::vector<VefpAppObject> vectorField{};
		int gridCount = 40;
		for (int i = 0; i < gridCount; i++) {
			for (int j = 0; j < gridCount; j++) {
				auto vf = VefpAppObject::createAppObject();
				vf.transform2d.scale = glm::vec2(0.005f);
				vf.transform2d.translation = {
					-1.0f + (i + 0.5f) * 2.0f / gridCount,
					-1.0f + (j + 0.5f) * 2.0f / gridCount };
				vf.color = glm::vec3(1.0f);
				vf.model = squareModel;
				vectorField.push_back(std::move(vf));
			}
		}

		GravityPhysicsSystem gravitySystem{ .81 };
		Vec2FieldSystem vecFieldSystem{};

		SimpleRenderSystem simpleRenderSystem(vefpDevice, vefpRenderer.getSwapChainRenderPass());

		while (!vefpWindow.shouldClose()) {
			glfwPollEvents();

			if (auto commandBuffer = vefpRenderer.beginFrame()) {
				//update systems
				gravitySystem.update(physicsObjects, 1.f / 60, 5);
				vecFieldSystem.update(gravitySystem, physicsObjects, vectorField);

				//render systems
				vefpRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderAppObjects(commandBuffer, physicsObjects);
				simpleRenderSystem.renderAppObjects(commandBuffer, vectorField);
				vefpRenderer.endSwapChainRenderPass(commandBuffer);
				vefpRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(vefpDevice.device());
	}

	void FirstApp::loadAppObjects() {
		std::vector<VefpModel::Vertex> vertices{
			{ { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
			{ { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
			{ { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } }
		};
		auto vefpModel = std::make_shared<VefpModel>(vefpDevice, vertices);

		auto triangle = VefpAppObject::createAppObject();
		triangle.model = vefpModel;
		triangle.color = { .1f, .8f, .1f };
		triangle.transform2d.translation.x = .2f;
		triangle.transform2d.scale = { 2.f, .5f };
		triangle.transform2d.rotation = .25f * glm::two_pi<float>();

		appObjects.push_back(std::move(triangle));
	}

}