#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include "Mesh.h"
#include "IRenderable.h"

namespace Engine
{
    namespace Rendering
    {
        class Model : public IRenderable
        {
            std::vector<Mesh> meshes;
        public:
            Model()
            {}
            explicit Model(const std::vector<Mesh>& meshes)
            {
                this->meshes = meshes;
            }
            void Draw(const IRenderingEngine& rendering_engine) override;
        };
    }
}
#endif // MODEL_H
