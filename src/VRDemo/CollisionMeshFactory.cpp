#include "CollisionMeshFactory.hpp"
#include "CollisionMesh.hpp"
#include "Model.h"
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include "ContentLoader.h"
CollisionMeshFactory::CollisionMeshFactory(Engine::Content::ContentLoader& manager)
	: m_manager(manager)
{
	
}

CollisionMesh & CollisionMeshFactory::Load(const std::string &path)
{
	// if the resource has not been loaded
	if (resources.find(path) == resources.end())
	{
		if (LoadCollisionMesh(path))
		{
			return resources[path];
		}

		std::cout << "\t" << "Failed loading collision mesh" << std::endl;

	}
	return resources[path];
}

bool CollisionMeshFactory::LoadCollisionMesh(const std::string &path) 
{
	auto& model = m_manager.LoadModel(path);
	auto& meshes = model.GetMeshes();
	btTriangleMesh triangle_mesh;
	std::vector<float*> vpos_data;
	for (auto& mesh : meshes)
	{
		auto& verts = mesh.GetVertices();
		std::vector<float> vpos;
		for (int i = 0; i < verts.size(); ++i)
		{
			vpos.push_back(verts[i].Position.x);
			vpos.push_back(verts[i].Position.y);
			vpos.push_back(verts[i].Position.z);
		}
		auto& indices = mesh.GetIndices();
		btIndexedMesh indexed_mesh;
		//indexed_mesh.m_indexType = PHY_INTEGER;
		indexed_mesh.m_numTriangles = indices.size() / 3;
		indexed_mesh.m_numVertices = vpos.size()/3;
		indexed_mesh.m_triangleIndexStride = 3*sizeof(int);
		indexed_mesh.m_vertexStride = 3*sizeof(float);
		indexed_mesh.m_vertexType = PHY_FLOAT;
		indexed_mesh.m_triangleIndexBase = reinterpret_cast<const unsigned char*>(mesh.GetIndices().data());
		float* data = new float[vpos.size()];
		std::copy(vpos.begin(), vpos.end(), data);
		indexed_mesh.m_vertexBase = reinterpret_cast<unsigned char*>(data);
		triangle_mesh.addIndexedMesh(indexed_mesh);
		vpos_data.push_back(data);
	}
	resources.emplace(path, triangle_mesh);
	resources[path].vpos_data = vpos_data;
	return true;
}

