#ifndef COLLISION_MESH_LOADER_HPP
#define COLLISION_MESH_LOADER_HPP


#include "CollisionMesh.hpp"
#include <map>

namespace Engine {namespace Content {
	class ContentLoader;
}
}

class CollisionMeshFactory
{
public:
	explicit CollisionMeshFactory(Engine::Content::ContentLoader& manager);
	CollisionMesh &Load(const std::string &path);

private:
	bool LoadCollisionMesh(const std::string &path);
	std::map<std::string, CollisionMesh> resources;
	Engine::Content::ContentLoader& m_manager;
};

#endif // COLLISION_MESH_LOADER_HPP
