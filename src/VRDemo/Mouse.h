#ifndef MOUSE_HPP
#define MOUSE_HPP

#include "MouseState.h"

/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: Code is from Stuart Adams' individual coursework for Advanced Graphics
*/

namespace Engine
{
    namespace Input
    {
        class MouseStateBuilder
        {
            glm::vec2 position;
        public:
            MouseState Build() const
            {
                return MouseState(position);
            }

            void SetRelativeMotion(const glm::vec2& position)
            {
                this->position = position;
            }
        };

        /**
        * \brief XNA-style mouse class. Call "GetState" to return the current state of the mouse.
        */
        class Mouse
        {
            static MouseState state;
            static MouseState UpdateState();
        public:
            static const MouseState& GetState();
        };
    }
}

#endif // MOUSE_HPP
