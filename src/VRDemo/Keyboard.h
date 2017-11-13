#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include "KeyboardState.h"

namespace Engine
{
    namespace Input
    {
        class KeyboardStateBuilder
        {
            std::unordered_map<Key, bool> keys;
        public:
            void Reset()
            {
                keys.clear();
            }

            void Add(const Key key, const bool keystate)
            {
                keys[key] = keystate;
            }

            KeyboardState Build() const
            {
                return KeyboardState(keys);
            }
        };

        /**
         * \brief XNA-style keyboard class. Call "GetState" to return the current state of the keyboard.
         */
        class Keyboard
        {
            static KeyboardState state;
            static KeyboardState UpdateState();
        public:
            static const KeyboardState& GetState();
        };
    }
}

#endif // KEYBOARD_HPP