#ifndef BUTTON_H
#define BUTTON_H

namespace Engine
{
    namespace Input
    {
        /**
         * \brief A button for use in boolean logic.
         */
        class Button
        {
            bool value;
            bool pressed;
        public:
            operator bool() const;
            Button();
            explicit Button(const bool value);
            void Down();
            void Up();
        };
    }
}

#endif // BUTTON_H