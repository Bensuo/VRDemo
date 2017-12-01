#ifndef ISKYBOX_H
#define ISKYBOX_H
#include "IRenderable.h"

/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: Code is from Stuart Adams' individual coursework for Advanced Graphics
*/

namespace Engine
{
    class ISkybox : public Rendering::IRenderable
    {
    public:
        virtual ~ISkybox() = default;
        void Draw(Rendering::IRenderingEngine& rendering_engine) override = 0;
        virtual unsigned GetVao() const = 0;
        virtual unsigned GetTexture() const = 0;
    };
}
#endif // ISKYBOX_H
