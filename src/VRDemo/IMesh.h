#ifndef IMESH_H
#define IMESH_H
#include <assimp/types.h>
#include <glm/detail/type_vec3.hpp>
#include "IRenderingEngine.h"
#include <vector>
#include "Material.h"

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
        struct Vertex
        {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec2 TexCoords;
            glm::vec4 Color;
            glm::vec3 Tangent;
            glm::vec3 BiTangent;
        };

        struct Texture
        {
            unsigned Id;
            std::string Type;
            aiString Path;
        };

        class IMesh
        {
        public:
            virtual ~IMesh() = default;
            virtual void Draw(const IRenderingEngine& rendering_engine) = 0;
            virtual const std::vector<Texture>& GetTextures() const = 0;
            virtual const std::vector<GLuint>& GetIndices() const = 0;
            virtual const Material& GetMaterial() const = 0;
        };
    }
}
#endif // IMESH_H
