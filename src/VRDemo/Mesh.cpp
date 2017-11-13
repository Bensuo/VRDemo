#include "Mesh.h"

namespace Engine
{
    namespace Rendering
    {
        Mesh::Mesh(const std::vector<Vertex>& vertices,
            const std::vector<GLuint>& indices,
            const std::vector<Texture>& textures,
            const Material& materials)
            : vertices(vertices),
            indices(indices),
            textures(textures),
            material(materials)
        {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glGenBuffers(1, &ebo);

            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

            // vertex positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(0));
            // vertex normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
            // vertex texture coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
            // vertex tangents
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Tangent)));
            // vertex bitangents
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, BiTangent)));

            glBindVertexArray(0);
        }

        const std::vector<Texture>& Mesh::GetTextures() const
        {
            return this->textures;
        }

        const Material& Mesh::GetMaterial() const
        {
            return this->material;
        }

        void Mesh::Draw(const IRenderingEngine& rendering_engine)
        {
            glBindVertexArray(vao);
            rendering_engine.Draw(this);
            glBindVertexArray(0);
        }

        const std::vector<GLuint>& Mesh::GetIndices() const
        {
            return indices;
        }
    }
}