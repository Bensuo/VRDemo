#include "Button.h"

namespace Engine
{
    namespace Input
    {
        Button::operator bool() const
        {
            return value;
        }

        Button::Button()
            : value(false),
            pressed(false)
        {}

        Button::Button(const bool value)
            : value(value),
            pressed(false)
        {}

        void Button::Down()
        {
            pressed = true;
        }

        void Button::Up()
        {
            if (pressed)
            {
                value = !value;
            }
            pressed = false;
        }
    }
}
