/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: This code was developed as part of our Computer Games Tech Project
*/
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
		auto scale = transform->GetScale();
		shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
		btRigidBody::btRigidBodyConstructionInfo info(mass, motion, shape, fallIntertia);
		m_rigid_body = new btRigidBody(info);

	}
	~RigidBodySphere(){}
private:

};

#endif // RIGID_BODY_SPHERE_HPP