#include "Model.h"

namespace Engine
{
    void Rendering::Model::Draw(IRenderingEngine& rendering_engine)
    {
		rendering_engine.PushMatrix();
		rendering_engine.TranslateMatrix(transform.GetPosition());
		rendering_engine.RotateMatrix(transform.GetRotation());
		rendering_engine.ScaleMatrix(transform.GetScale());
        for (GLuint i = 0; i < this->meshes.size(); i++)
            meshes[i].Draw(rendering_engine);
		rendering_engine.PopMatrix();
    }
}