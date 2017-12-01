#ifndef SKYBOX_H
#define SKYBOX_H
#include <GL/glew.h>
#include "ISkybox.h"

/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: Code is from Stuart Adams' individual coursework for Advanced Graphics
*/

namespace Engine
{
    namespace Rendering
    {
        /**
         * \brief Skybox class. Hides all the implementation detail of an OpenGL skybox.
         */
        class Skybox : public ISkybox
        {
            const float size = 1;
            GLuint vbo;
            GLuint vao;
            GLuint texture;
        public:
            Skybox() : vbo(0), vao(0), texture(0)
            {
            }

            Skybox(const GLuint vbo, const GLuint vao, const GLuint texture)
            {
                this->vbo = vbo;
                this->vao = vao;
                this->texture = texture;
            }

            ~Skybox();

            void Draw(IRenderingEngine& rendering_engine) override;

            unsigned GetVao() const override { return vao; }
            unsigned GetTexture() const override { return texture; }
        };
    }
}
#endif // SKYBOX_H
