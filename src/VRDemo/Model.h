#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include "Mesh.h"
#include "IRenderable.h"
#include "Transform3D.hpp"

namespace Engine
{
    namespace Rendering
    {
        class Model : public IRenderable
        {
            std::vector<Mesh> meshes;
			Transform3D transform;
        public:
            Model()
            {}
            explicit Model(const std::vector<Mesh>& meshes)
            {
                this->meshes = meshes;
            }
            void Draw(IRenderingEngine& rendering_engine) override;
			Transform3D& GetTransform()
			{
				return transform;
			}
        };
    }
}
#endif // MODEL_H
