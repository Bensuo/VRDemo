#ifndef MOUSESTATE_HPP
#define MOUSESTATE_HPP

#include <glm/detail/type_vec2.hpp>

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
        class MouseState
        {
            glm::vec2 relative_motion;
        public:
            const glm::vec2& RelativeMotion() const;
            MouseState(const glm::vec2& relative_motion);
            MouseState();
        };
    }
}
#endif // MOUSESTATE_HPP
