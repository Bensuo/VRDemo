/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: This code was developed as part of our Computer Games Tech Project
*/
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
