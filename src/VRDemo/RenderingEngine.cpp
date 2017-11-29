#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include "RenderingEngine.h"
#include "Mesh.h"
#include "Skybox.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Extras/OVR_Math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"
#include <glm/gtx/quaternion.hpp>
#include "ShaderFactory.h"

namespace Engine
{
    namespace Rendering
    {
        RenderingEngine::RenderingEngine(std::shared_ptr<VRSystem> system)
			: vr_system(system)
        {
            version = std::string(reinterpret_cast<char const*>(glGetString(GL_VERSION)));

            glewExperimental = GL_TRUE;
            if (glewInit() != GLEW_OK)
                std::cout << "GLEW not initialized correctly" << std::endl;

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
			
            shadow_map_data.DepthShader = Engine::Content::ShaderFactory::Load("res/shaders/shadow_depth.vs", "res/shaders/shadow_depth.fs");

            glGenFramebuffers(1, &shadow_map_data.DepthMapFBO);
            // create depth texture
            glGenTextures(1, &shadow_map_data.DepthMap);
            glBindTexture(GL_TEXTURE_2D, shadow_map_data.DepthMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_map_data.ShadowWidth, shadow_map_data.ShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            // attach depth texture as FBO's depth buffer
            glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_data.DepthMapFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map_data.DepthMap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        const Shader& RenderingEngine::DepthShader()
        {
            return shadow_map_data.DepthShader;
        }

        const std::string& RenderingEngine::ApiVersion()
        {
            return version;
        }

        void RenderingEngine::ClearScreen() const
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glClearColor(0 / 255.0, 0 / 255.0, 0 / 255.0, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

	    void RenderingEngine::ClearEyeBuffer(int eye)
        {
			vr_system->ClearEyeBuffer(eye);
        }

        void RenderingEngine::Begin(const Shader& shader)
        {
            this->current_shader = shader;
            PushMatrix();
        }

	    void RenderingEngine::Begin(const glm::mat4& view, const glm::mat4& perspective, const glm::vec3& position, const Shader& shader)
        {
            this->current_shader = shader;
            this->view = view;
            this->current_shader.Use();
            this->current_shader.SetMat4("projection", perspective);
            this->current_shader.SetMat4("view", view);
            this->current_shader.SetVec3("viewPos", position);

            this->current_shader.Use();
            this->current_shader.SetMat4("lightSpaceMatrix", shadow_map_data.LightSpaceMatrix);
            glActiveTexture(GL_TEXTURE30);
            glBindTexture(GL_TEXTURE_2D, shadow_map_data.DepthMap);

            PushMatrix();
        }

        void RenderingEngine::End()
        {
            current_shader.Disable();
            PopMatrix();
        }

        void RenderingEngine::Draw(const Mesh* mesh) const
        {
            current_shader.SetMat4("model", matrix_stack.top());
            current_shader.SetBool("normal_mapped", false);
            // Bind appropriate textures
            GLuint diffuse_nr = 1;
            GLuint specular_nr = 1;
            GLuint normal_nr = 1;
            for (GLuint i = 0; i < mesh->GetTextures().size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
                const auto name = mesh->GetTextures()[i].Type;
                current_shader.SetUint(name.c_str(), i);

                if (name == "normalMap")
                {
                    current_shader.SetBool("normal_mapped", true);
                }

                glBindTexture(GL_TEXTURE_2D, mesh->GetTextures()[i].Id);
            }

            current_shader.SetMaterial(&mesh->GetMaterial());

            glDrawElements(GL_TRIANGLES, mesh->GetIndices().size(), GL_UNSIGNED_INT, nullptr);

            // Always good practice to set everything back to defaults once configured.
            for (GLuint i = 0; i < mesh->GetTextures().size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }

        void RenderingEngine::Draw(const Skybox* skybox) const
        {
            const auto view = glm::mat4(glm::mat3(this->view));
            current_shader.SetMat4("view", view);

            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);

            glBindVertexArray(skybox->GetVao());
            glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetTexture());
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
        }

        void RenderingEngine::PushMatrix()
        {
            matrix_stack.push(glm::mat4(1.0));
        }

        void RenderingEngine::PushMatrix(const glm::mat4& matrix)
        {
            matrix_stack.push(matrix);
        }

        void RenderingEngine::PopMatrix()
        {
            if (matrix_stack.size() >= 0)
                matrix_stack.pop();
        }

        void RenderingEngine::TranslateMatrix(const glm::vec3& position)
        {
            if (matrix_stack.size() == 0)
                return;
            matrix_stack.top() = translate(matrix_stack.top(), position);
        }

        void RenderingEngine::ScaleMatrix(const glm::vec3& scale)
        {
            if (matrix_stack.size() == 0)
                return;
            matrix_stack.top() = glm::scale(matrix_stack.top(), scale);
        }

        void RenderingEngine::RotateMatrix(const glm::quat& axis)
        {
            MultiplyMatrix(glm::toMat4(axis));
        }

        void RenderingEngine::MultiplyMatrix(const glm::mat4& matrix)
        {
            if (matrix_stack.size() == 0)
                return;
            matrix_stack.top() *= matrix;
        }

        void RenderingEngine::AddLights(const std::vector<SpotLight>& lights) const
        {
            for (auto i = 0; i < lights.size(); ++i)
            {
                current_shader.SetSpotLight(lights[i], i);
            }
        }

        void RenderingEngine::AddLights(const std::vector<PointLight>& lights) const
        {
            for (auto i = 0; i < lights.size(); ++i)
            {
                current_shader.SetPointLight(lights[i], i);
            }
        }

        void RenderingEngine::AddLight(const SpotLight& light) const
        {
            current_shader.SetSpotLight(light);
        }

        void RenderingEngine::AddLight(const PointLight& light) const
        {
            current_shader.SetPointLight(light);
        }

        void RenderingEngine::AddLight(const DirectionalLight& light) const
        {
            current_shader.SetDirectionalLight(light);
        }

	    void RenderingEngine::EndRender()
        {
			vr_system->EndFrame();
			vr_system->RenderMirror(1280, 720);
        }

	    void RenderingEngine::BeginRender()
        {
			vr_system->BeginFrame();
        }

	    void RenderingEngine::Commit(int eye)
        {
			vr_system->CommitBuffer(eye);
        }

        glm::ivec2 RenderingEngine::ShadowMapSize() const
        {
            return glm::ivec2(shadow_map_data.ShadowWidth, shadow_map_data.ShadowHeight);
        }

        void RenderingEngine::SetViewport(const int x, const int y, const int width, const int height)
        {
            glViewport(x, y, width, height);
        }

        void RenderingEngine::BeginDepthPass(const glm::vec3& light_position, const glm::vec3& light_direction, const float near_plane, const float far_plane, const float fov)
        {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 lightProjection = glm::perspective(glm::radians(fov), (GLfloat)shadow_map_data.ShadowWidth / (GLfloat)shadow_map_data.ShadowHeight, near_plane, far_plane);

            glm::mat4 lightView = glm::lookAt(light_position, light_position + light_direction, glm::vec3(0.0, 1.0, 0.0));
            
            shadow_map_data.LightSpaceMatrix = lightProjection * lightView;

            shadow_map_data.DepthShader.Use();
            shadow_map_data.DepthShader.SetMat4("lightSpaceMatrix", shadow_map_data.LightSpaceMatrix);

            glViewport(0, 0, shadow_map_data.ShadowWidth, shadow_map_data.ShadowHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_data.DepthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
        }

        void RenderingEngine::EndDepthPass() const
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glCullFace(GL_BACK);
        }
    }
}
