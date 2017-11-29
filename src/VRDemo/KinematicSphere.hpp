#ifndef KINEMATIC_SPHERE_HPP
#define KINEMATIC_SPHERE_HPP

#include <btBulletDynamicsCommon.h>
#include "RigidBody.hpp"
#include <glm/glm.hpp>
#include "Transform3D.hpp"
#include "PhysicsMotionState.hpp"

class KinematicSphere : public RigidBody
{
public:
	KinematicSphere(float radius, Transform3D* transform)
	{
		btCollisionShape* shape = new btSphereShape(radius);
		btMotionState* motion = new PhysicsMotionState(transform, "");
		auto scale = transform->GetScale();
		shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
		btRigidBody::btRigidBodyConstructionInfo info(0, motion, shape);
		m_rigid_body = new btRigidBody(info);
		m_rigid_body->setCollisionFlags(m_rigid_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		m_rigid_body->setActivationState(DISABLE_DEACTIVATION);
		

	}
	~KinematicSphere() {}
private:

};
#endif // KINEMATIC_SPHERE_HPP
