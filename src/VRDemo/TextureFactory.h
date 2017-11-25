#ifndef TEXTURE_FACTORY_H
#define TEXTURE_FACTORY_H
#include <map>
namespace Engine
{
	namespace Content
	{
		class TextureFactory
		{
		public:
			TextureFactory() {}
			Rendering::Texture& Load(const std::string& path)
			{
				// if the resource has not been loaded
				if (resources.find(path) == resources.end())
				{
					if (LoadTexture(path))
					{
						return resources[path];
					}

					std::cout << "\t" << "Failed to load texture" << std::endl;

				}
				return resources[path];
			}
			bool LoadTexture(const std::string& path)
			{
				Rendering::Texture texture;
				GLuint textureID;
				glGenTextures(1, &textureID);
				int width, height;
				auto components = 4;
				const auto image = stbi_load(path.c_str(), &width, &height, &components, 4);

				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glBindTexture(GL_TEXTURE_2D, 0);
				stbi_image_free(image);
				texture.Id = textureID;
				texture.Path = path;
				resources.emplace(path, texture);
				return true;
			}
		private:
			std::map<std::string, Rendering::Texture> resources;

		};
	}
}


#endif // TEXTURE_FACTORY_H
