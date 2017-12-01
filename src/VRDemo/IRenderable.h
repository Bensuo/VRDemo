
#ifndef IRENDERABLE_H
#define IRENDERABLE_H

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
        class IRenderingEngine;
        class IRenderable
        {
        public:
            virtual ~IRenderable() = default;
            virtual void Draw(IRenderingEngine& rendering_engine) = 0;
        };
    }
}
#endif // IRENDERABLE_H
