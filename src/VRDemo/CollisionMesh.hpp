#ifndef COLLISION_MESH_HPP
#define COLLISION_MESH_HPP

#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <vector>
class CollisionMesh
{
public:
	CollisionMesh(){}
	CollisionMesh(btTriangleMesh& mesh)
		: m_mesh(mesh)
	{
		
	}
	~CollisionMesh()
	{
		for (int i = 0; i < vpos_data.size(); ++i)
		{
			if(vpos_data[i])
			delete[] vpos_data[i];
		}
	}
	btTriangleMesh m_mesh;
	std::vector<float*> vpos_data;
};

#endif // COLLISION_MESH_HPP
