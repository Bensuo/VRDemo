#ifndef BUTTON_H
#define BUTTON_H

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
