#ifndef MODELFACTORY_H
#define MODELFACTORY_H

#include "Model.h"
#include <assimp/material.h>
#include <assimp/scene.h>
#include <iostream>
#include <assimp/Importer.hpp>
#include "ModelFactory.h"
#include "stb_image.h"
#include <iostream>
#include <assimp/postprocess.h>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Engine
{
    namespace Content
    {
        class ModelFactory
        {
        public:
            Rendering::Model& Load(const std::string& path)
            {
                meshes.clear();

                // if the resource has not been loaded
                if (resources.find(path) == resources.end())
                {
                    if (LoadModel(path))
                    {
                        resources.emplace(path, Rendering::Model(meshes));
                        return resources[path];
                    }

                    std::cout << "\t" << "Loading error mesh as substitute." << std::endl;

                    LoadModel("res/models/error.obj");
                    resources.emplace(path, Rendering::Model(meshes));
                }
                return resources[path];
            }

        private:
            std::unordered_map<std::string, Rendering::Model> resources;
            std::vector<Rendering::Mesh> meshes;

            bool LoadModel(std::string path)
            {
                std::cout << "Loading model: " << path << std::endl;

                Assimp::Importer import;
                const auto scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

                if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
                {
                    std::cout << "\t" << "Error loading model: " << import.GetErrorString() << std::endl;
                    return false;
                }
                const auto directory = path.substr(0, path.find_last_of('/'));

                ProcessNode(scene->mRootNode, scene, directory);

                return true;
            }

            void ProcessNode(aiNode* node, const aiScene* scene, const std::string& directory)
            {
                for (GLuint i = 0; i < node->mNumMeshes; i++)
                {
                    const auto mesh = scene->mMeshes[node->mMeshes[i]];
                    meshes.push_back(ProcessMesh(mesh, scene, directory));
                }
                for (GLuint i = 0; i < node->mNumChildren; i++)
                {
                    ProcessNode(node->mChildren[i], scene, directory);
                }
            }

            static Rendering::Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory)
            {
                std::vector<Rendering::Vertex> vertices;
                std::vector<GLuint> indices;
                std::vector<Rendering::Texture> textures;
                Material material;

                for (GLuint i = 0; i < mesh->mNumVertices; i++)
                {
                    Rendering::Vertex vertex;
                    glm::vec4 vector;

                    vector.x = mesh->mVertices[i].x;
                    vector.y = mesh->mVertices[i].y;
                    vector.z = mesh->mVertices[i].z;
                    vertex.Position = vector;

                    if (mesh->mNormals)
                    {
                        vector.x = mesh->mNormals[i].x;
                        vector.y = mesh->mNormals[i].y;
                        vector.z = mesh->mNormals[i].z;
                        vertex.Normal = vector;
                    }

                    if (mesh->mTextureCoords[0])
                    {
                        glm::vec2 vec;
                        vec.x = mesh->mTextureCoords[0][i].x;
                        vec.y = mesh->mTextureCoords[0][i].y;
                        vertex.TexCoords = vec;
                    }
                    else
                    {
                        vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                    }

                    if (mesh->mColors[0])
                    {
                        vertex.Color.r = mesh->mColors[0][i].r;
                        vertex.Color.g = mesh->mColors[0][i].g;
                        vertex.Color.b = mesh->mColors[0][i].b;
                        vertex.Color.a = mesh->mColors[0][i].a;
                    }

                    if (mesh->mTangents)
                    {
                        vector.x = mesh->mTangents[i].x;
                        vector.y = mesh->mTangents[i].y;
                        vector.z = mesh->mTangents[i].z;
                        vertex.Tangent = vector;
                    }

                    if (mesh->mBitangents)
                    {
                        vector.x = mesh->mBitangents[i].x;
                        vector.y = mesh->mBitangents[i].y;
                        vector.z = mesh->mBitangents[i].z;
                        vertex.BiTangent = vector;
                    }

                    vertices.push_back(vertex);
                }

                for (GLuint i = 0; i < mesh->mNumFaces; i++)
                {
                    const auto face = mesh->mFaces[i];
                    for (GLuint j = 0; j < face.mNumIndices; j++)
                        indices.push_back(face.mIndices[j]);
                }

                if (mesh->mMaterialIndex >= 0)
                {
                    const auto ai_material = scene->mMaterials[mesh->mMaterialIndex];
                    auto diffuse_maps = LoadMaterialTextures(ai_material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
                    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
                    auto specular_maps = LoadMaterialTextures(ai_material, aiTextureType_SPECULAR, "texture_specular", directory);
                    textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
                    auto normal_maps = LoadMaterialTextures(ai_material, aiTextureType_HEIGHT, "texture_normals", directory);
                    textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

                    aiString ai_name;
                    ai_material->Get(AI_MATKEY_NAME, ai_name);

                    aiColor3D ai_diffuse;
                    glm::vec3 diffuse;
                    if (AI_SUCCESS == ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, ai_diffuse))
                    {
                        diffuse.r = ai_diffuse.r;
                        diffuse.g = ai_diffuse.g;
                        diffuse.b = ai_diffuse.b;
                    }

                    aiColor3D ai_ambient;
                    glm::vec3 ambient;
                    if (AI_SUCCESS == ai_material->Get(AI_MATKEY_COLOR_AMBIENT, ai_ambient))
                    {
                        ambient.r = ai_ambient.r;
                        ambient.g = ai_ambient.g;
                        ambient.b = ai_ambient.b;
                    }

                    aiColor3D ai_specular;
                    glm::vec3 specular;
                    if (AI_SUCCESS == ai_material->Get(AI_MATKEY_COLOR_SPECULAR, ai_specular))
                    {
                        specular.r = ai_specular.r;
                        specular.g = ai_specular.g;
                        specular.b = ai_specular.b;
                    }

                    float shininess;
                    ai_material->Get(AI_MATKEY_SHININESS, shininess);

                    material.Diffuse = diffuse;
                    material.Shininess = shininess;
                    material.Specular = specular;
                    material.Ambient = ambient;
                }

                return Rendering::Mesh(vertices, indices, textures, material);
            }

            static std::vector<Rendering::Texture> LoadMaterialTextures(aiMaterial* mat, const aiTextureType type, const std::string& type_name, const std::string& directory)
            {
                std::vector<Rendering::Texture> textures;
                for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
                {
                    aiString str;
                    mat->GetTexture(type, i, &str);
                    GLboolean skip = false;
                    for (GLuint j = 0; j < textures.size(); j++)
                    {
                        if (textures[j].Path == str)
                        {
                            textures.push_back(textures[j]);
                            skip = true;
                            break;
                        }
                    }
                    if (!skip)
                    {
                        std::cout << "\t" << "Loading model texture: " << str.C_Str() << std::endl;

                        Rendering::Texture texture;
                        texture.Id = TextureFromFile(str.C_Str(), directory);
                        texture.Type = type_name;
                        texture.Path = str;
                        textures.push_back(texture);
                    }
                }
                return textures;
            }

            static GLint TextureFromFile(const char* path, const std::string& directory)
            {
                auto filename = std::string(path);
                filename = directory + '/' + filename;
                GLuint textureID;
                glGenTextures(1, &textureID);
                int width, height;
                auto components = 4;
                const auto image = stbi_load(filename.c_str(), &width, &height, &components, 4);

                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(image);
                return textureID;
            }
        };
    }
}
#endif // MODELFACTORY_H
