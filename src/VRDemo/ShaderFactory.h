#ifndef SHADERFACTORY_H
#define SHADERFACTORY_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "Shader.h"

namespace Engine
{
    namespace Content
    {
        class ShaderFactory
        {
        public:
            Rendering::Shader& Load(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path = "")
            {
                auto path = vertex_path + ", " + fragment_path;

                if (geometry_path != "")
                {
                    path += ", " + geometry_path;
                }

                // if the resource has not been loaded
                if (resources.find(path) == resources.end())
                {
                    std::string vertex_code;
                    std::string fragment_code;
                    std::string geometry_code;

                    std::ifstream v_shader_file;
                    std::ifstream f_shader_file;
                    std::ifstream g_shader_file;

                    v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                    f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                    g_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

                    try
                    {
                        v_shader_file.open(vertex_path);
                        f_shader_file.open(fragment_path);

                        std::stringstream v_shader_stream;
                        std::stringstream f_shader_stream;

                        v_shader_stream << v_shader_file.rdbuf();
                        f_shader_stream << f_shader_file.rdbuf();

                        v_shader_file.close();
                        f_shader_file.close();

                        vertex_code = v_shader_stream.str();
                        fragment_code = f_shader_stream.str();

                        if (geometry_path != "")
                        {
                            g_shader_file.open(geometry_path);
                            std::stringstream g_shader_stream;
                            g_shader_stream << g_shader_file.rdbuf();
                            g_shader_file.close();
                            geometry_code = g_shader_stream.str();
                        }
                    }
                    catch (std::ifstream::failure e)
                    {
                        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
                    }

                    auto v_shader_code = vertex_code.c_str();
                    auto f_shader_code = fragment_code.c_str();
                    auto g_shader_code = geometry_code.c_str();
                    std::cout << "\t" << "Loading shader: " << path << std::endl;

                    GLint success;
                    GLchar infoLog[512];

                    const auto vertex = glCreateShader(GL_VERTEX_SHADER);
                    glShaderSource(vertex, 1, &v_shader_code, nullptr);
                    glCompileShader(vertex);

                    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
                    if (!success)
                    {
                        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
                        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
                    }

                    const auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
                    glShaderSource(fragment, 1, &f_shader_code, nullptr);
                    glCompileShader(fragment);

                    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
                    if (!success)
                    {
                        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
                        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
                    }

                    const auto geometry = glCreateShader(GL_GEOMETRY_SHADER);
                    if (geometry_path != "")
                    {
                        glShaderSource(geometry, 1, &g_shader_code, nullptr);
                        glCompileShader(geometry);

                        glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
                        if (!success)
                        {
                            glGetShaderInfoLog(geometry, 512, nullptr, infoLog);
                            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
                        }
                    }

                    const auto program = glCreateProgram();
                    glAttachShader(program, vertex);
                    glAttachShader(program, fragment);
                    if (geometry_path != "")
                    {
                        glAttachShader(program, geometry);
                    }
                    glLinkProgram(program);

                    glGetProgramiv(program, GL_LINK_STATUS, &success);
                    if (!success)
                    {
                        glGetProgramInfoLog(program, 512, nullptr, infoLog);
                        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
                    }

                    glDeleteShader(vertex);
                    glDeleteShader(fragment);
                    glDeleteShader(geometry);

                    resources.emplace(path, Rendering::Shader(program));
                }
                return resources[path];
            }
        private:
            std::unordered_map<std::string, Rendering::Shader> resources;
        };
    }
}
#endif // SHADERFACTORY_H
