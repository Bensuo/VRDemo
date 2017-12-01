
#ifndef SKYBOXFACTORY_H
#define SKYBOXFACTORY_H

#include "Skybox.h"
#include <GL/glew.h>
#include "SkyboxFactory.h"
#include <iostream>
#include "stb_image.h"
#include <unordered_map>
#include <filesystem>

/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: Code is from Stuart Adams' individual coursework for Advanced Graphics
*/

namespace Engine
{
    namespace Content
    {
        class SkyboxFactory
        {
            std::unordered_map<std::string, Rendering::Skybox> resources;

            static bool LoadCubeMapSide(GLuint& texture, const GLenum target, const std::string& path)
            {
                std::cout << "\t" << "Loading skybox texture: " << path << std::endl;

                glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

                int x, y, n;
                const auto image_data = stbi_load(path.c_str(), &x, &y, &n, STBI_rgb_alpha);
                if (!image_data) {
                    fprintf(stderr, "ERROR: could not load %s\n", path.c_str());
                    return false;
                }

                if ((x & x - 1) != 0 || (y & y - 1) != 0) {
                    fprintf(stderr,
                        "WARNING: image %s is not power-of-2 dimensions\n",
                        path.c_str());
                }

                glTexImage2D(
                    target,
                    0,
                    GL_RGBA,
                    x,
                    y,
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    image_data);
                stbi_image_free(image_data);
                return true;
            }
        public:
            Rendering::Skybox& Load(const std::string& path_right,
                const std::string& path_left,
                const std::string& path_top,
                const std::string& path_bottom,
                const std::string& path_back,
                const std::string& path_front)
            {
                auto key = path_right + path_left + path_top + path_bottom + path_back + path_front;

                // if the resource has not been loaded
                if (resources.find(key) == resources.end())
                {
                    const auto SIZE = 1;
                    GLuint vbo;
                    GLuint vao;
                    GLuint texture;

                    GLfloat skyboxVertices[] =
                    {
                        -SIZE,  SIZE, -SIZE,
                        -SIZE, -SIZE, -SIZE,
                        SIZE, -SIZE, -SIZE,
                        SIZE, -SIZE, -SIZE,
                        SIZE,  SIZE, -SIZE,
                        -SIZE,  SIZE, -SIZE,

                        -SIZE, -SIZE,  SIZE,
                        -SIZE, -SIZE, -SIZE,
                        -SIZE,  SIZE, -SIZE,
                        -SIZE,  SIZE, -SIZE,
                        -SIZE,  SIZE,  SIZE,
                        -SIZE, -SIZE,  SIZE,

                        SIZE, -SIZE, -SIZE,
                        SIZE, -SIZE,  SIZE,
                        SIZE,  SIZE,  SIZE,
                        SIZE,  SIZE,  SIZE,
                        SIZE,  SIZE, -SIZE,
                        SIZE, -SIZE, -SIZE,

                        -SIZE, -SIZE,  SIZE,
                        -SIZE,  SIZE,  SIZE,
                        SIZE,  SIZE,  SIZE,
                        SIZE,  SIZE,  SIZE,
                        SIZE, -SIZE,  SIZE,
                        -SIZE, -SIZE,  SIZE,

                        -SIZE,  SIZE, -SIZE,
                        SIZE,  SIZE, -SIZE,
                        SIZE,  SIZE,  SIZE,
                        SIZE,  SIZE,  SIZE,
                        -SIZE,  SIZE,  SIZE,
                        -SIZE,  SIZE, -SIZE,

                        -SIZE, -SIZE, -SIZE,
                        -SIZE, -SIZE,  SIZE,
                        SIZE, -SIZE, -SIZE,
                        SIZE, -SIZE, -SIZE,
                        -SIZE, -SIZE,  SIZE,
                        SIZE, -SIZE,  SIZE
                    };

                    glGenBuffers(1, &vbo);
                    glBindBuffer(GL_ARRAY_BUFFER, vbo);
                    glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &skyboxVertices, GL_STATIC_DRAW);

                    glGenVertexArrays(1, &vao);
                    glBindVertexArray(vao);
                    glEnableVertexAttribArray(0);
                    glBindBuffer(GL_ARRAY_BUFFER, vbo);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

                    // generate a cube-map texture to hold all the sides	
                    glGenTextures(1, &texture);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

                    std::cout << "Loading skybox:" << std::endl;

                    // load each image and copy into a side of the cube-map texture
                    LoadCubeMapSide(texture, GL_TEXTURE_CUBE_MAP_POSITIVE_X, path_right);
                    LoadCubeMapSide(texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, path_left);
                    LoadCubeMapSide(texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, path_top);
                    LoadCubeMapSide(texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, path_bottom);
                    LoadCubeMapSide(texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, path_back);
                    LoadCubeMapSide(texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, path_front);

                    // format cube map texture
                    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

                    resources.emplace(key, Rendering::Skybox(vbo, vao, texture));
                }
                return resources[key];
            }
        };
    }
}
#endif // SKYBOXFACTORY_H
