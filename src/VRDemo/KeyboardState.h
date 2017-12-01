#ifndef KEYBOARDSTATE_HPP
#define KEYBOARDSTATE_HPP

#include <unordered_map>

/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: Code is from Stuart Adams' individual coursework for Advanced Graphics
*/

namespace Engine
{
    enum Key
    {
        Key_1,
        Key_2,
        Key_3,
        Key_4,
        Key_5,
        Key_6,
        Key_7,
        Key_8,
        Key_9,
        Key_0,
        Key_A,
        Key_B,
        Key_C,
        Key_D,
        Key_E,
        Key_F,
        Key_G,
        Key_H,
        Key_I,
        Key_J,
        Key_K,
        Key_L,
        Key_M,
        Key_N,
        Key_O,
        Key_P,
        Key_Q,
        Key_R,
        Key_S,
        Key_T,
        Key_U,
        Key_V,
        Key_W,
        Key_Y,
        Key_X,
        Key_Z,
        Key_Up,
        Key_Down,
        Key_Left,
        Key_Right,
        Key_Shift,
        Key_Space,
        Key_Escape
    };

    namespace Input
    {
        class KeyboardState
        {
            std::unordered_map<Key, bool> keys;
            void SetKeyUp(const Key key);
            void SetKeyDown(const Key key);
        public:
            KeyboardState();
            explicit KeyboardState(const std::unordered_map<Key, bool>);
            ~KeyboardState();
            bool IsKeyUp(const Key key) const;
            bool IsKeyDown(const Key key) const;
        };
    }
}
#endif // KEYBOARDSTATE_HPP
