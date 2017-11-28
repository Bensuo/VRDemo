#ifndef RIGID_BODY_SPHERE_HPP
#define RIGID_BODY_SPHERE_HPP
#include <btBulletDynamicsCommon.h>
#include "RigidBody.hpp"
#include <glm/glm.hpp>
#include "Transform3D.hpp"
#include "PhysicsMotionState.hpp"

class RigidBodySphere : public RigidBody
{
public:
	RigidBodySphere(float radius, float mass, Transform3D* transform)
	{
		btCollisionShape* shape = new btSphereShape(radius);
		btMotionState* motion = new PhysicsMotionState(transform, "");
		btVector3 fallIntertia(0, 0, 0);
		shape->calculateLocalInertia(mass, fallIntertia);
		btRigidBody::btRigidBodyConstructionInfo info(mass, motion, shape, fallIntertia);
		m_rigid_body = new btRigidBody(info);
		m_rigid_body->activate();
	}
	~RigidBodySphere(){}
private:

};

#endif // RIGID_BODY_SPHERE_HPP