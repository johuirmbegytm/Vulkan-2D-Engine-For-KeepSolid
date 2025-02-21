#pragma once

#include "vefp_model.hpp"

#include <memory>

namespace vefp {
	// new
	struct RigidBody2dComponent {
		glm::vec2 velocity;
		float mass{ 1.0f };
	};

	struct Transform2dComponent {
		glm::vec2 translation{}; // position offset
		glm::vec2 scale{ 1.f, 1.f };
		float rotation;
		
		glm::mat2 mat2() { 
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{ {c, s}, {-s, c} };

			glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} };
			return rotMatrix * scaleMat;
		}
	};

	class VefpAppObject {
	public:
		using id_t = unsigned int;

		static VefpAppObject createAppObject() {
			static id_t currentId = 0;
			return VefpAppObject{ currentId++ };
		}

		VefpAppObject(const VefpAppObject&) = delete;
		VefpAppObject& operator=(const VefpAppObject&) = delete;
		VefpAppObject(VefpAppObject&&) = default;
		VefpAppObject& operator=(VefpAppObject&&) = default;

		id_t getId() { return id; }

		std::shared_ptr<VefpModel> model{};
		glm::vec3 color{};
		Transform2dComponent transform2d{};
		RigidBody2dComponent rigidBody2d{};

	private:
		VefpAppObject(id_t objId) : id{ objId } {}

		id_t id;
	};
}