/*
Name: Stuart D. Adams
Student ID: B00265262
I declare that I produced the following code for the GED module and that this is my own work.
I am aware of the penalties incurred by submitting in full or in part work that is not our own and that was developed by third parties that are not appropriately acknowledged.
*/
#include "ShaderFactory.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include "Shader.h"

namespace Engine
{
	namespace Content
	{
		std::shared_ptr<Resource> ShaderFactory::Load(std::string path)
		{
			std::string vertexCode;
			std::string fragmentCode;

			GLuint program;

			std::ifstream vShaderFile;
			std::ifstream fShaderFile;

			vShaderFile.exceptions(std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::badbit);
			try
			{
				vShaderFile.open(path + ".vs");
				fShaderFile.open(path + ".fs");
				
				std::stringstream vShaderStream;
				std::stringstream fShaderStream;
				
				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();
				
				vShaderFile.close();
				fShaderFile.close();

				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
			}
			catch (std::ifstream::failure e)
			{
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
			}

			auto vShaderCode = vertexCode.c_str();
			auto fShaderCode = fragmentCode.c_str();
			std::cout << "Loading shader:" << std::endl;
			std::cout << "\t" << "Loading vertex shader: " << path << ".vs" << std::endl;
			std::cout << "\t" << "Loading fragment shader: " << path << ".fs" << std::endl;

			GLuint vertex, fragment;
			GLint success;
			GLchar infoLog[512];

			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, nullptr);
			glCompileShader(vertex);

			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			}

			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, nullptr);
			glCompileShader(fragment);

			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			}

			program = glCreateProgram();
			glAttachShader(program, vertex);
			glAttachShader(program, fragment);
			glLinkProgram(program);

			glGetProgramiv(program, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(program, 512, nullptr, infoLog);
				std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			}

			glDeleteShader(vertex);
			glDeleteShader(fragment);

			std::cout << std::endl;
			return std::make_shared<Rendering::Shader>(program);
		}
	}
}
