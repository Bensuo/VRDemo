#include "KeyboardState.h"

namespace Engine
{
    namespace Input
    {
        KeyboardState::KeyboardState()
        {

        }

        KeyboardState::KeyboardState(const std::unordered_map<Key, bool> keys)
            : keys(keys)
        {

        }

        KeyboardState::~KeyboardState()
        {

        }

        bool KeyboardState::IsKeyUp(const Key key) const
        {
            const auto& it = keys.find(key);

            if (it != keys.end())
            {
                return it->second;
            }

            return false;
        }

        bool KeyboardState::IsKeyDown(const Key key) const
        {
            const auto& it = keys.find(key);

            if (it != keys.end())
            {
                return it->second;
            }

            return false;
        }

        void KeyboardState::SetKeyUp(const Key key)
        {
            keys[key] = false;
        }

        void KeyboardState::SetKeyDown(const Key key)
        {
            keys[key] = true;
        }
    }
}
