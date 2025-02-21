#pragma once

#include "simple_render_system.hpp"


namespace vefp {

	class GravityPhysicsSystem {

	public:

		GravityPhysicsSystem(float strength) : strengthGravity{ strength } {}

	    const float strengthGravity;

		void update(std::vector<VefpAppObject>& objs, float dt, unsigned int substeps);
		glm::vec2 computeForce(VefpAppObject& fromObj, VefpAppObject& toObj) const;
		
	private:
		
		void stepSimulation(std::vector<VefpAppObject>& physicsObjs, float dt);

	};

	class Vec2FieldSystem {
	
	public:
		void update(
			const GravityPhysicsSystem& physicsSystem,
			std::vector<VefpAppObject>& physicsObjs,
			std::vector<VefpAppObject>& vectorField);
	};

}
