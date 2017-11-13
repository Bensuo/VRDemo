
#ifndef IRENDERABLE_H
#define IRENDERABLE_H

namespace Engine
{
    namespace Rendering
    {
        class IRenderingEngine;
        class IRenderable
        {
        public:
            virtual ~IRenderable() = default;
            virtual void Draw(const IRenderingEngine& rendering_engine) = 0;
        };
    }
}
#endif // IRENDERABLE_H
