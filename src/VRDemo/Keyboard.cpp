#include "Keyboard.h"
#include <SDL.h>

namespace Engine
{
    namespace Input
    {
        KeyboardState Keyboard::state;

        KeyboardState Keyboard::UpdateState()
        {
            KeyboardStateBuilder state_builder;

            SDL_PumpEvents();
            const auto& state = SDL_GetKeyboardState(nullptr);

            if (state[SDL_SCANCODE_W])
            {
                state_builder.Add(Key_W, true);
            }
            else
            {
                state_builder.Add(Key_W, false);
            }

            if (state[SDL_SCANCODE_A])
            {
                state_builder.Add(Key_A, true);
            }
            else
            {
                state_builder.Add(Key_A, false);
            }

            if (state[SDL_SCANCODE_D])
            {
                state_builder.Add(Key_D, true);
            }
            else
            {
                state_builder.Add(Key_D, false);
            }

            if (state[SDL_SCANCODE_UP])
            {
                state_builder.Add(Key_Up, true);
            }
            else
            {
                state_builder.Add(Key_Up, false);
            }

            if (state[SDL_SCANCODE_DOWN])
            {
                state_builder.Add(Key_Down, true);
            }
            else
            {
                state_builder.Add(Key_Down, false);
            }

            if (state[SDL_SCANCODE_LEFT])
            {
                state_builder.Add(Key_Left, true);
            }
            else
            {
                state_builder.Add(Key_Left, false);
            }

            if (state[SDL_SCANCODE_RIGHT])
            {
                state_builder.Add(Key_Right, true);
            }
            else
            {
                state_builder.Add(Key_Right, false);
            }

            if (state[SDL_SCANCODE_S])
            {
                state_builder.Add(Key_S, true);
            }
            else
            {
                state_builder.Add(Key_S, false);
            }

            if (state[SDL_SCANCODE_F])
            {
                state_builder.Add(Key_F, true);
            }
            else
            {
                state_builder.Add(Key_F, false);
            }

            if (state[SDL_SCANCODE_ESCAPE])
            {
                state_builder.Add(Key_Escape, true);
            }
            else
            {
                state_builder.Add(Key_Escape, false);
            }

            if (state[SDL_SCANCODE_SPACE])
            {
                state_builder.Add(Key_Space, true);
            }
            else
            {
                state_builder.Add(Key_Space, false);
            }

            if (state[SDL_SCANCODE_LSHIFT])
            {
                state_builder.Add(Key_Shift, true);
            }
            else
            {
                state_builder.Add(Key_Shift, false);
            }

            if (state[SDL_SCANCODE_1])
            {
                state_builder.Add(Key_1, true);
            }
            else
            {
                state_builder.Add(Key_1, false);
            }

            if (state[SDL_SCANCODE_2])
            {
                state_builder.Add(Key_2, true);
            }
            else
            {
                state_builder.Add(Key_2, false);
            }

            if (state[SDL_SCANCODE_3])
            {
                state_builder.Add(Key_3, true);
            }
            else
            {
                state_builder.Add(Key_3, false);
            }

            if (state[SDL_SCANCODE_4])
            {
                state_builder.Add(Key_4, true);
            }
            else
            {
                state_builder.Add(Key_4, false);
            }

            return state_builder.Build();
        }

        const KeyboardState& Keyboard::GetState()
        {
            state = UpdateState();
            return state;
        }
    }
}
