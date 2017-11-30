#ifndef RIGID_BODY_PLANE_HPP
#define RIGID_BODY_PLANE_HPP
#include "RigidBody.hpp"
#include <glm/glm.hpp>
#include "PhysicsMotionState.hpp"

class RigidBodyPlane : public RigidBody
{
public:
	RigidBodyPlane(glm::vec3 normal, float scalar)
	{
		btCollisionShape* shape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), scalar);
		btMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
		btRigidBody::btRigidBodyConstructionInfo info(0, motion, shape);
		m_rigid_body = new btRigidBody(info);
	}
	~RigidBodyPlane(){}
private:

};

#endif // RIGID_BODY_PLANE_HPP
