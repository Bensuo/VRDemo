/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: This code was developed as part of our Computer Games Tech Project
*/

#ifndef RIGID_BODY_MESH_HPP
#define RIGID_BODY_MESH_HPP
#include "RigidBody.hpp"
#include "CollisionMesh.hpp"


class RigidBodyMesh : public RigidBody
{
public:
	RigidBodyMesh(CollisionMesh& mesh, glm::vec3 scale, glm::vec3 pos)
	{
		auto shape = new btBvhTriangleMeshShape(&mesh.m_mesh, true);
		btCollisionShape* shape2 = new btScaledBvhTriangleMeshShape(shape, btVector3(scale.x, scale.y, scale.z));
		btMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z)));
		btRigidBody::btRigidBodyConstructionInfo info(0, motion, shape2);
		m_rigid_body = new btRigidBody(info);
	}
};


#endif // RIGID_BODY_MESH_HPP
