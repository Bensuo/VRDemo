#ifndef COLLISION_MESH_HPP
#define COLLISION_MESH_HPP

#include <BulletCollision/CollisionShapes/btTriangleMesh.h>

class CollisionMesh
{
public:
	CollisionMesh(){}
	CollisionMesh(btTriangleMesh& mesh)
		: m_mesh(mesh)
	{
		
	}

	btTriangleMesh m_mesh;
};

#endif // COLLISION_MESH_HPP
