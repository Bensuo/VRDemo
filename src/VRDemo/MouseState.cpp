#include "MouseState.h"

namespace Engine
{
    namespace Input
    {
        const glm::vec2& MouseState::RelativeMotion() const
        {
            return relative_motion;
        }

        MouseState::MouseState(const glm::vec2& relative_motion)
            : relative_motion(relative_motion)
        {

        }

        MouseState::MouseState()
        {
        }
    }
}
