#ifndef SHADER_H
#define SHADER_H
#include <GL/glew.h>
#include <string>
#include "IShader.h"
#include "Material.h"

namespace Engine
{
    namespace Rendering
    {
        struct DirectionalLight;
        struct SpotLight;
        struct PointLight;

        /**
         * \brief Hides the implementation detail of an OpenGL shader.
         */
        class Shader : public IShader
        {
            GLuint program;
            std::string name;
        public:
            Shader() : program(0) {}

            explicit Shader(const GLuint program) { this->program = program; }
            void Use() const override;
            void Disable() const override;

            void SetUint(const std::string& uniform, const unsigned unsigned_integer) const override;
            void SetInt(const std::string& uniform, const int integer) const override;
            void SetFloat(const std::string& uniform, const float floating_point) const override;
            void SetMat4(const std::string& uniform, const glm::mat4& matrix) const override;
            void SetMat3(const std::string& uniform, const glm::mat3& matrix) const override;
            void SetVec4(const std::string& uniform, const glm::vec4& vector) const override;
            void SetVec3(const std::string& uniform, const glm::vec3& vector) const override;
            void SetVec2(const std::string& uniform, const glm::vec2& vector) const override;

            void SetPointLight(const PointLight& light, const std::string& uniform_name = "point_light") const;
            void SetPointLight(const PointLight& light, int index, const std::string& uniform_name = "point_light") const;

            void SetSpotLight(const SpotLight& spot_light, const std::string& uniform_name = "spot_light") const;
            void SetSpotLight(const SpotLight& spot_light, const int index, const std::string& uniform_name = "spot_lights") const;
            
            void SetDirectionalLight(const DirectionalLight& directional_light) const;

            void SetMaterial(const Material* light) const;
            void SetBool(const std::string& uniform, bool boolean) const;

            GLuint Program() override;
        };
    }
}
#endif // SHADER_H
