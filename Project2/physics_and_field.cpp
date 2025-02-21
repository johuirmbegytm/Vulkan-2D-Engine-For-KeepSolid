#include "physics_and_field.hpp";
#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <stdexcept>

namespace vefp {

	void GravityPhysicsSystem::update(std::vector<VefpAppObject>& objs, float dt, unsigned int substeps) {
	
		const float stepDelta = dt / substeps;
		for (int i = 0; i < substeps; ++i) {
			stepSimulation(objs, stepDelta);
		}
	}

	glm::vec2 GravityPhysicsSystem::computeForce(VefpAppObject& fromObj, VefpAppObject& toObj) const {
		auto offset = fromObj.transform2d.translation - toObj.transform2d.translation;
		float distranceSquared = glm::dot(offset, offset);


		// clown town - just going to return 0 if objects are too close together...
		if (glm::abs(distranceSquared) < 1e-10f) {
			return { .0f, .0f };
		}

		float force = strengthGravity * toObj.rigidBody2d.mass * fromObj.rigidBody2d.mass / distranceSquared;

		return force * offset / glm::sqrt(distranceSquared);
	}

	void GravityPhysicsSystem::stepSimulation(std::vector<VefpAppObject>& physicsObjs, float dt) {
		// Loops through all pairs of objects and applies attractive force between them
		for (auto iterA = physicsObjs.begin(); iterA != physicsObjs.end(); ++iterA) {
			auto& objA = *iterA;
			for (auto iterB = iterA; iterB != physicsObjs.end(); ++iterB) {
				if (iterA == iterB) continue;
				auto& objB = *iterB;

				auto force = computeForce(objA, objB);
				objA.rigidBody2d.velocity += dt * -force / objA.rigidBody2d.mass;
				objB.rigidBody2d.velocity += dt * force / objB.rigidBody2d.mass;
			}
		}

		for (auto& obj : physicsObjs) {
			obj.transform2d.translation += dt * obj.rigidBody2d.velocity;
		}
	}

	void Vec2FieldSystem::update(
		const GravityPhysicsSystem& physicsSystem,
		std::vector<VefpAppObject>& physicsObjs,
		std::vector<VefpAppObject>& vectorField) 
	{
		for (auto& vf : vectorField) {
			glm::vec2 direction{};
			for (auto& obj : physicsObjs) {
				direction += physicsSystem.computeForce(obj, vf);
			}

			vf.transform2d.scale.x = 0.005f + 0.045f * glm::clamp(glm::log(glm::length(direction) + 1) / 3.f, 0.f, 1.f);
			vf.transform2d.rotation = atan2(direction.y, direction.x);
		}
	}

}