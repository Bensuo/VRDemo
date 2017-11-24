#pragma once

#include "ISkybox.h"
#include <glm/mat4x2.hpp>
#include <string>
#include <glm/gtc/quaternion.hpp>

namespace Engine
{
    namespace Rendering
    {
        class Camera;
        class Skybox;
        class Shader;
        class Mesh;

        class IRenderingEngine
        {
        public:
            virtual ~IRenderingEngine() {}

            virtual const std::string& ApiVersion() = 0;
            virtual void ClearScreen() const = 0;
			virtual void ClearEyeBuffer(int eye) = 0;
            virtual void Draw(const Mesh* mesh) const = 0;
            virtual void Draw(const Skybox* skybox) const = 0;

            virtual void PushMatrix() = 0;
            virtual void PushMatrix(const glm::mat4& matrix) = 0;
            virtual void PopMatrix() = 0;

            virtual void TranslateMatrix(const glm::vec3& position) = 0;
            virtual void ScaleMatrix(const glm::vec3& scale) = 0;
            virtual void RotateMatrix(const glm::quat& axis) = 0;
            virtual void MultiplyMatrix(const glm::mat4& matrix) = 0;

            virtual void End() = 0;
			virtual void EndRender() = 0;
			virtual void BeginRender() = 0;
            virtual void Begin(const glm::mat4& view, const glm::mat4& perspective, const glm::vec3& position, const Shader& shader) = 0;
			virtual void Commit(int eye) = 0;
        };
    }
}
