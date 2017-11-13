#include "Window.h"
#include <GL/glew.h>


namespace Engine
{
    namespace Windowing
    {
        Window::Window(const int width, const int height, const std::string& title)
        {
            //init SDL
            if (SDL_Init(SDL_INIT_VIDEO) < 0)
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
                return;
            }

            //set our OpenGL version
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

            //core profile
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

            //init double buffering with a 24-bit Z-buffer
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

            window = SDL_CreateWindow(
                title.c_str(), //window title
                SDL_WINDOWPOS_CENTERED, //initial x position
                SDL_WINDOWPOS_CENTERED, //initial y position
                width, //width, in pixels
                height, //height, in pixels
                SDL_WINDOW_OPENGL
            );

            gl_context = SDL_GL_CreateContext(window);
            SDL_GL_MakeCurrent(window, gl_context);

            glEnable(GL_MULTISAMPLE);
        }

        Window::~Window()
        {
            SDL_GL_DeleteContext(gl_context);
            SDL_DestroyWindow(window);
        }

        bool Window::WindowInitialised() const
        {
            auto result = true;
            if (!window)
                result = false;
            return result;
        }

        glm::vec2 Window::GetPosition() const
        {
            int x, y;
            SDL_GetWindowPosition(window, &x, &y);
            return glm::vec2(x, y);
        }

        glm::vec2 Window::GetResolution() const
        {
            int width, height;
            SDL_GetWindowSize(window, &width, &height);
            return glm::vec2(width, height);
        }

        glm::vec2 Window::GetMaxResolution() const
        {
            return max_window_size;
        }

        void Window::SetHeight(const unsigned height) const
        {
            int current_width, current_height;

            if (centred)
            {
                SDL_SetWindowPosition(window, GetPosition().x, GetPosition().y - (height - GetResolution().y) / 2);
            }

            SDL_GetWindowSize(window, &current_width, &current_height);
            SetSize(current_width, height);
        }

        void Window::SetWidth(const unsigned width) const
        {
            int current_width, current_height;

            if (centred)
            {
                SDL_SetWindowPosition(window, GetPosition().x - (width - GetResolution().x) / 2, GetPosition().y);
            }

            SDL_GetWindowSize(window, &current_width, &current_height);
            SetSize(width, current_width);
        }

        void Window::SetSize(const unsigned width, const unsigned height) const
        {
            if (centred)
            {
                SDL_SetWindowPosition(window, GetPosition().x - (width - GetResolution().x) / 2, GetPosition().y - (height - GetResolution().y) / 2);
            }
            SDL_SetWindowSize(window, width, height);
        }

        void Window::SetPosition(const glm::vec2& position)
        {
            centred = false;
            SDL_SetWindowPosition(window, position.x, position.y);
        }

        void Window::SetBorderless()
        {
            borderless = !borderless;
            if (borderless)
            {
                SDL_SetWindowBordered(window, SDL_FALSE);
            }
            else
            {
                SDL_SetWindowBordered(window, SDL_TRUE);
            }
        }

        void Window::SetGrab()
        {
            window_grab = !window_grab;
            if (window_grab)
            {
                SDL_SetWindowGrab(window, SDL_TRUE);
            }
            else
            {
                SDL_SetWindowGrab(window, SDL_FALSE);
            }
            SDL_ShowCursor(SDL_DISABLE);
        }

        void Window::SetFullscreen()
        {
            full_screen = !full_screen;
            if (full_screen)
            {
                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
            }
            else
            {
                SDL_SetWindowFullscreen(window, 0);
            }
        }

        void Window::Maximise() const
        {
            SDL_MaximizeWindow(window);
        }

        void Window::Minimise() const
        {
            SDL_MinimizeWindow(window);
        }

        void Window::Hide() const
        {
            SDL_HideWindow(window);
        }

        float Window::GetScale() const
        {
            const auto resoltution = GetResolution();
            return resoltution.y / max_window_size.y;
        }

        void Window::SwapBuffer() const
        {
            SDL_GL_SwapWindow(window);
        }

        void Window::Update() const
        {
            SDL_Event event;
            while (SDL_PollEvent(&event) != 0)
            {
                if (event.type == SDL_QUIT)
                {
                    OnClose();
                }
            }
        }
    }
}
