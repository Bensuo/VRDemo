#ifndef RIGID_BODY_BOX_HPP
#define RIGID_BODY_BOX_HPP

#include <btBulletDynamicsCommon.h>
#include "RigidBody.hpp"
#include <glm/glm.hpp>
#include "Transform3D.hpp"
#include "PhysicsMotionState.hpp"

class RigidBodyBox : public RigidBody
{
public:
	RigidBodyBox(glm::vec3 half_widths, float mass, Transform3D* transform, std::string name)
	{
		btCollisionShape* shape = new btBoxShape(btVector3(half_widths.x, half_widths.y, half_widths.z));
		btMotionState* motion = new PhysicsMotionState(transform, name);
		btVector3 fallIntertia(0, 0, 0);
		shape->calculateLocalInertia(mass, fallIntertia);
		auto scale = transform->GetScale();
		shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
		btRigidBody::btRigidBodyConstructionInfo info(mass, motion, shape, fallIntertia);
		m_rigid_body = new btRigidBody(info);
		//m_rigid_body->activate();
	}
	~RigidBodyBox() {}
private:

};

#endif // RIGID_BODY_BOX_HPP
