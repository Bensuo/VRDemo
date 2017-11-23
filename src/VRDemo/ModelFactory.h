#ifndef MODELFACTORY_H
#define MODELFACTORY_H

#include "Model.h"
#include <assimp/material.h>
#include <assimp/scene.h>

#include <unordered_map>
#include <glm/glm.hpp>


namespace Engine
{
    namespace Content
    {
	    class ContentLoader;

	    class ModelFactory
        {
        public:
	        explicit ModelFactory(ContentLoader& content);
	        Rendering::Model &Load(const std::string &path);


        private:
            std::unordered_map<std::string, Rendering::Model> resources;
            std::vector<Rendering::Mesh> meshes;
			ContentLoader& content;
	        bool LoadModel(std::string path);

	        void ProcessNode(aiNode *node, const aiScene *scene, const std::string &directory);

	        Rendering::Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene, const std::string &directory);

	        std::vector<Rendering::Texture> LoadMaterialTextures(aiMaterial *mat, const aiTextureType type, const std::string &type_name, const std::string &directory);


        };
    }
}
#endif // MODELFACTORY_H
