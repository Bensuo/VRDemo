#pragma once

#include <stack>
#include "IRenderingEngine.h"
#include "Shader.h"
#include "Light.h"
#include <memory>
#include <vector>
#include <OVR_CAPI_GL.h>
#include "TextureBuffer.h"
#include "VRSystem.h"

namespace Engine
{
    namespace Rendering
    {
        struct ShadowMapData
        {
            Shader DepthShader;
            const unsigned int ShadowWidth = 1024, ShadowHeight = 1024;
            unsigned int DepthMapFBO, DepthMap;
            glm::mat4 LightSpaceMatrix;
        };

        /**
         * \brief Abstracts OpenGL API calls.
         */
        class RenderingEngine : public IRenderingEngine
        {
            ShadowMapData shadow_map_data;
            Shader current_shader;
            std::string version;
        public:
            RenderingEngine(std::shared_ptr<VRSystem> system);

            const std::string& ApiVersion() override;

            void ClearScreen() const override;
			void ClearEyeBuffer(int eye) override;

            void Begin(const Shader & shader);

            void Draw(const Mesh* mesh) const override;
            void Draw(const Skybox* skybox) const override;

            const Shader& DepthShader();
            void Begin(const glm::mat4& view, const glm::mat4& perspective, const glm::vec3& position, const Shader& shader) override;
            void End() override;

            void PushMatrix() override;
            void PushMatrix(const glm::mat4& matrix) override;
            void PopMatrix() override;
            void TranslateMatrix(const glm::vec3& position) override;
            void ScaleMatrix(const glm::vec3& scale) override;
            void RotateMatrix(const glm::quat& axis) override;
            void MultiplyMatrix(const glm::mat4& matrix) override;

            void AddLights(const std::vector<SpotLight>& lights) const;
            void AddLights(const std::vector<PointLight>& lights) const;
            void AddLight(const SpotLight& light) const;
            void AddLight(const PointLight& light) const;
            void AddLight(const DirectionalLight& light) const;
			void EndRender() override;
	        void BeginRender() override;
	        void Commit(int eye) override;

            glm::ivec2 ShadowMapSize() const;

            void SetViewport(const int x, const int y, const int width, const int height);

            void BeginDepthPass(const glm::vec3& light_position, const glm::vec3& light_direction, const float near_plane, const float far_plane, const float fov);
            void EndDepthPass() const;
        private:
            std::stack<glm::mat4> matrix_stack;
            glm::mat4 view;

			//VR
            std::shared_ptr<VRSystem> vr_system;
        };
    }
}
