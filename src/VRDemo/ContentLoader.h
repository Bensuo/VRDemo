#ifndef CONTENT_LOADER_H
#define CONTENT_LOADER_H

/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: Code is based on Stuart Adams' individual coursework for Advanced Graphics but has been extended for this project's new features
*/

#include "SkyboxFactory.h"
#include "ModelFactory.h"
#include "ShaderFactory.h"
#include "TextureFactory.h"
#include "CollisionMeshFactory.hpp"

namespace Engine
{
    namespace Content
    {
        /**
         * \brief Simple content loader. Abstracts asset importing implementation.
         */
        class ContentLoader
        {
            ShaderFactory shader_factory;
            SkyboxFactory skybox_factory;
            ModelFactory model_factory{*this};
			TextureFactory texture_factory;
			CollisionMeshFactory collision_mesh_factory{*this};

        public:
            /**
             * \brief Load a new 3D model.
             * \param path The path to the 3D model asset
             * \return The imported 3D model for use in OpenGL
             */
            Rendering::Model& LoadModel(const std::string& path)
            {
                return model_factory.Load(path);
            }

            /**
             * \brief Load a new Skybox
             * \param path_right The path to the right skybox image
             * \param path_left The path to the lef skybox image
             * \param path_top The path to the top skybox image
             * \param path_bottom The path to the bottom skybox image
             * \param path_back The path to the back skybox image
             * \param path_front The path to the front skybox image
             * \return The imported skybox for use in OpenGL
             */
            Rendering::Skybox& LoadSkybox(const std::string& path_right,
                const std::string& path_left,
                const std::string& path_top,
                const std::string& path_bottom,
                const std::string& path_back,
                const std::string& path_front)
            {
                return skybox_factory.Load(path_right, path_left, path_top, path_bottom, path_back, path_front);
            }

            /**
             * \brief Load an OpenGL shader
             * \param vertex The path to the vertex shader
             * \param fragment The path to the fragment shader
             * \param geometry The path to the geometry shader. (Optional)
             * \return The imported shader for use in OpenGL
             */
            Rendering::Shader& LoadShader(const std::string& vertex, const std::string& fragment, const std::string& geometry = "")
            {
                return shader_factory.Load(vertex, fragment, geometry);
            }

			Rendering::Texture& LoadTexture(const std::string& path)
            {
				return texture_factory.Load(path);
            }
			CollisionMesh& LoadCollisionMesh(const std::string& path)
            {
				return collision_mesh_factory.Load(path);
            }
        };
    }
}
#endif // CONTENT_LOADER_H
