#include "Skybox.h"
#include "IRenderingEngine.h"

namespace Engine
{
    namespace Rendering
    {
        void Skybox::Draw(IRenderingEngine& rendering_engine)
        {
            rendering_engine.Draw(this);
        }

        Skybox::~Skybox()
        {
        }
    }
}