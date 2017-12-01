#pragma once

#include <SDL.h>
#include <glm/detail/type_vec2.hpp>
#include "IWindow.h"
#include <string>

/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: Code is from Stuart Adams' individual coursework for Advanced Graphics
*/

namespace Engine
{
    namespace Windowing
    {
        /**
         * \brief Hides the implementation detail of an SDL window
         */
        class Window : public IWindow
        {
        public:
            Window(int width, int height, const std::string& title);
            ~Window();
            bool WindowInitialised() const override;

            glm::vec2 GetPosition() const override;
            glm::vec2 GetResolution() const override;
            glm::vec2 GetMaxResolution() const override;

            void SetHeight(const unsigned height) const override;
            void SetWidth(const unsigned width) const override;
            void SetSize(const unsigned width, const unsigned height) const override;
            void SetPosition(const glm::vec2& position) override;

            void SetGrab() override;
            void SetBorderless() override;
            void SetFullscreen() override;
            void Maximise() const override;
            void Minimise() const override;
            void Hide() const override;

            float GetScale() const override;

            void SwapBuffer() const override;

            void Update() const override;
        private:
            const glm::vec2 max_window_size = glm::vec2(1920, 1080);
            bool full_screen = false;
            bool window_grab = false;
            bool centred = true;
            bool borderless = false;
            SDL_Window* window = nullptr;
            SDL_Event sdl_event;
            SDL_GLContext gl_context = nullptr;
        };
    }
}
