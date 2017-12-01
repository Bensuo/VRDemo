#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include "IRenderingEngine.h"
#include "IMesh.h"

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
         * \brief A 3D mesh. Contains all
         */
        class Mesh : public IMesh
        {
        public:
            Mesh(const std::vector<Vertex>& vertices,
                const std::vector<GLuint>& indices,
                const std::vector<Texture>& textures,
                const Material& material);

            const std::vector<Texture>& GetTextures() const override;
            const std::vector<GLuint>& GetIndices() const override;
            const Material& GetMaterial() const override;
            void Draw(const IRenderingEngine& rendering_engine) override;
            std::vector<Vertex>& GetVertices()
            {
                return vertices;
            }
        private:
            GLuint vao, vbo, ebo;
            std::vector<Vertex> vertices;
            std::vector<GLuint> indices;
            std::vector<Texture> textures;
            Material material;
        };
    }
}
#endif // MESH_H
