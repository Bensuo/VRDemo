#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include "RenderingEngine.h"
#include "Mesh.h"
#include "Skybox.h"
#include <glm/glm.hpp>
#include <Extras/OVR_Math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"
#include <glm/gtx/quaternion.hpp>

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
            this->shader = shader;
            PushMatrix();
        }

	    void RenderingEngine::Begin(const glm::mat4& view, const glm::mat4& perspective, const glm::vec3& position, const Shader& shader)
        {
            this->shader = shader;
            this->view = view;
            this->shader.Use();
            this->shader.SetMat4("projection", perspective);
            this->shader.SetMat4("view", view);
            this->shader.SetVec3("viewPos", position);
            PushMatrix();
        }

        void RenderingEngine::End()
        {
            shader.Disable();
            PopMatrix();
        }

        void RenderingEngine::Draw(const Mesh* mesh) const
        {
            shader.SetMat4("model", matrix_stack.top());
            shader.SetBool("normal_mapped", false);
            // Bind appropriate textures
            GLuint diffuse_nr = 1;
            GLuint specular_nr = 1;
            GLuint normal_nr = 1;
            for (GLuint i = 0; i < mesh->GetTextures().size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
                const auto name = mesh->GetTextures()[i].Type;
                shader.SetUint(name.c_str(), i);

                if (name == "normalMap")
                {
                    shader.SetBool("normal_mapped", true);
                }

                glBindTexture(GL_TEXTURE_2D, mesh->GetTextures()[i].Id);
            }

            shader.SetMaterial(&mesh->GetMaterial());

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
            shader.SetMat4("view", view);

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
                shader.SetSpotLight(lights[i], i);
            }
        }

        void RenderingEngine::AddLights(const std::vector<PointLight>& lights) const
        {
            for (auto i = 0; i < lights.size(); ++i)
            {
                shader.SetPointLight(lights[i], i);
            }
        }

        void RenderingEngine::AddLight(const SpotLight& light) const
        {
            shader.SetSpotLight(light);
        }

        void RenderingEngine::AddLight(const PointLight& light) const
        {
            shader.SetPointLight(light);
        }

        void RenderingEngine::AddLight(const DirectionalLight& light) const
        {
            shader.SetDirectionalLight(light);
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
    }
}
