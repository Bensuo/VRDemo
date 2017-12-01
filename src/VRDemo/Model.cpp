#include "Model.h"

namespace Engine
{
    void Rendering::Model::Draw(IRenderingEngine& rendering_engine)
    {
        for (GLuint i = 0; i < this->meshes.size(); i++)
            meshes[i].Draw(rendering_engine);
    }
}