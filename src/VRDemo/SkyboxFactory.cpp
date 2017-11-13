/*
Name: Stuart D. Adams
Student ID: B00265262
I declare that I produced the following code for the GED module and that this is my own work.
I am aware of the penalties incurred by submitting in full or in part work that is not our own and that was developed by third parties that are not appropriately acknowledged.
*/
#include "SkyboxFactory.h"
#include <iostream>
#include "stb_image.h"
#include "Skybox.h"

namespace Engine
{
	namespace Content
	{
		bool SkyboxFactory::LoadCubeMapSide(GLuint& texture, GLenum target, const char* path) const
		{

		}

		std::shared_ptr<Resource> SkyboxFactory::Load(std::string path)
		{
			auto SIZE = 1;
			GLuint vbo;
			GLuint vao;
			GLuint texture;

			GLfloat skyboxVertices[] = {
				// Positions          
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
			LoadCubeMapSide(texture, GL_TEXTURE_CUBE_MAP_POSITIVE_X, (path + "_right.png").c_str());
			LoadCubeMapSide(texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, (path + "_left.png").c_str());
			LoadCubeMapSide(texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, (path + "_top.png").c_str());
			LoadCubeMapSide(texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, (path + "_bottom.png").c_str());
			LoadCubeMapSide(texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, (path + "_back.png").c_str());
			LoadCubeMapSide(texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, (path + "_front.png").c_str());

			// format cube map texture
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

			std::cout << std::endl;
			return std::make_shared<Rendering::Skybox>(vbo, vao, texture);
		}
	}
}
