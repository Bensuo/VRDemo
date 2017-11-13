#ifndef ISHADER_H
#define ISHADER_H
#include <string>
#include <glm/mat4x2.hpp>

namespace Engine
{
    class IShader
    {
    public:
        virtual ~IShader() = default;
        virtual void Use() const = 0;
        virtual void Disable() const = 0;
        virtual void SetUint(const std::string& uniform, const unsigned unsigned_integer) const = 0;
        virtual void SetInt(const std::string& uniform, const int integer) const = 0;
        virtual void SetFloat(const std::string& uniform, const float floating_point) const = 0;
        virtual void SetMat4(const std::string& uniform, const glm::mat4& matrix) const = 0;
        virtual void SetMat3(const std::string& uniform, const glm::mat3& matrix) const = 0;
        virtual void SetVec4(const std::string& uniform, const glm::vec4& vector) const = 0;
        virtual void SetVec3(const std::string& uniform, const glm::vec3& vector) const = 0;
        virtual void SetVec2(const std::string& uniform, const glm::vec2& vector) const = 0;
    };
}
#endif // ISHADER_H
