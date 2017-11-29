#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP
#include <btBulletDynamicsCommon.h>
#include "RigidBody.hpp"

namespace Physics
{
	class PhysicsEngine
	{
	public:
		PhysicsEngine();
		~PhysicsEngine();
		void SetGravity(float x, float y, float z);
		void AddRigidBody(RigidBody& rigid_body, int type, int collidesWith);
		void StepSimulation(float timeStep);
		void RemoveRigidBody(RigidBody& rigid_body);
		void SetMaxSubsteps(int val);
		void SetInternalTickCallback(btInternalTickCallback cb, void *worldUserInfo);
	private:
		btBroadphaseInterface* m_broadphase;
		btDefaultCollisionConfiguration* m_collision_configuration;
		btCollisionDispatcher* m_dispatcher;
		btSequentialImpulseConstraintSolver* m_solver;
		btDiscreteDynamicsWorld* m_dynamics_world;
		int m_max_substeps;
	};
}

#endif // PHYSICS_ENGINE_HPP
