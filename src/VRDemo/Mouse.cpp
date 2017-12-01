#include "Mouse.h"
#include <SDL.h>
#include <SDL_events.h>

namespace Engine
{
    namespace Input
    {
        MouseState Mouse::state;

        MouseState Mouse::UpdateState()
        {
            MouseStateBuilder state_builder;

            //SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
            //SDL_SetRelativeMouseMode(SDL_TRUE);

            SDL_Event event;
            SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
            //User requests quit
            if (event.type == SDL_MOUSEMOTION)
            {
                state_builder.SetRelativeMotion(glm::vec2(event.motion.xrel, -event.motion.yrel));
            }

            return state_builder.Build();
        }

        const MouseState& Mouse::GetState()
        {
            state = UpdateState();
            return state;
        }
    }
}
