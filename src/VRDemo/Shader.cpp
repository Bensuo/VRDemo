#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include "Material.h"
#include "Light.h"

namespace Engine
{
    namespace Rendering
    {
        // Uses the current shader
        void Shader::Use() const
        {
            glUseProgram(this->program);
        }

        void Shader::Disable() const
        {
            glUseProgram(0);
        }

        void Shader::SetUint(const std::string& uniform, const unsigned unsigned_integer) const
        {
            glUniform1i(glGetUniformLocation(program, uniform.c_str()), unsigned_integer);
        }

        void Shader::SetInt(const std::string& uniform, const int integer) const
        {
            glUniform1i(glGetUniformLocation(program, uniform.c_str()), integer);
        }

        void Shader::SetFloat(const std::string& uniform, const float floating_point) const
        {
            glUniform1fv(glGetUniformLocation(program, uniform.c_str()), 1, &floating_point);
        }

        void Shader::SetMat4(const std::string& uniform, const glm::mat4& matrix) const
        {
            glUniformMatrix4fv(glGetUniformLocation(program, uniform.c_str()), 1, GL_FALSE, value_ptr(matrix));
        }

        void Shader::SetMat3(const std::string& uniform, const glm::mat3& matrix) const
        {
            glUniformMatrix3fv(glGetUniformLocation(program, uniform.c_str()), 1, GL_FALSE, value_ptr(matrix));
        }

        void Shader::SetVec4(const std::string& uniform, const glm::vec4& vector) const
        {
            glUniform4fv(glGetUniformLocation(program, uniform.c_str()), 1, value_ptr(vector));
        }

        void Shader::SetVec3(const std::string& uniform, const glm::vec3& vector) const
        {
            glUniform3fv(glGetUniformLocation(program, uniform.c_str()), 1, value_ptr(vector));
        }

        void Shader::SetVec2(const std::string& uniform, const glm::vec2& vector) const
        {
            glUniform2fv(glGetUniformLocation(program, uniform.c_str()), 1, value_ptr(vector));
        }

        void Shader::SetPointLight(const PointLight& light, const std::string& uniform_name) const
        {
            SetVec3(uniform_name + ".ambient", light.Ambient);
            SetVec3(uniform_name + ".diffuse", light.Diffuse);
            SetVec3(uniform_name + ".position", light.Position);
            SetVec3(uniform_name + ".specular", light.Specular);
            SetFloat(uniform_name + ".constant", light.Constant);
            SetFloat(uniform_name + ".linear", light.Linear);
            SetFloat(uniform_name + ".quadratic", light.Quadratic);
        }

        void Shader::SetPointLight(const PointLight& light, const int index, const std::string& uniform_name) const
        {
            SetPointLight(light, uniform_name + "[" + std::to_string(index) + "]");
        }

        void Shader::SetSpotLight(const SpotLight& spot_light, const std::string& uniform_name) const
        {
            SetVec3(uniform_name + ".position", spot_light.Position);
            SetVec3(uniform_name + ".direction", spot_light.Direction);
            SetFloat(uniform_name + ".cutOff", spot_light.CutOff);
            SetFloat(uniform_name + ".outerCutOff", spot_light.OuterCutOff);

            SetFloat(uniform_name + ".constant", spot_light.Constant);
            SetFloat(uniform_name + ".linear", spot_light.Linear);
            SetFloat(uniform_name + ".quadratic", spot_light.Quadratic);

            SetVec3(uniform_name + ".ambient", spot_light.Ambient);
            SetVec3(uniform_name + ".diffuse", spot_light.Diffuse);
            SetVec3(uniform_name + ".specular", spot_light.Specular);
        }

        void Shader::SetSpotLight(const SpotLight& spot_light, const int index, const std::string& uniform_name) const
        {
            SetSpotLight(spot_light, uniform_name + "[" + std::to_string(index) + "]");
        }

        void Shader::SetDirectionalLight(const DirectionalLight& directional_light) const
        {
            SetVec3("directional_light.ambient", directional_light.Ambient);
            SetVec3("directional_light.diffuse", directional_light.Diffuse);
            SetVec3("directional_light.direction", directional_light.Direction);
            SetVec3("directional_light.specular", directional_light.Specular);
        }

        void Shader::SetBool(const std::string& uniform, const bool boolean) const
        {
            glUniform1i(glGetUniformLocation(program, uniform.c_str()), boolean);
        }

        void Shader::SetMaterial(const Material* material) const
        {
            SetVec3("material.ambient", material->Ambient);
            SetVec3("material.diffuse", material->Diffuse);
            SetVec3("material.specular", material->Specular);
            SetFloat("material.shininess", material->Shininess);
        }
    }
}
