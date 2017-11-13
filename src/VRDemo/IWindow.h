#pragma once
#include <glm/detail/type_vec2.hpp>
#include "Signal.h"

namespace Engine
{
    namespace Windowing
    {
        class IWindow
        {
        public:
            Signal<> OnClose;

            virtual ~IWindow() = default;

            virtual bool WindowInitialised() const = 0;

            virtual glm::vec2 GetPosition() const = 0;
            virtual glm::vec2 GetResolution() const = 0;
            virtual glm::vec2 GetMaxResolution() const = 0;

            virtual void SetHeight(const unsigned height) const = 0;
            virtual void SetWidth(const unsigned width) const = 0;
            virtual void SetSize(const unsigned width, const unsigned height) const = 0;
            virtual void SetPosition(const glm::vec2& position) = 0;

            virtual void SetGrab() = 0;
            virtual void SetBorderless() = 0;
            virtual void SetFullscreen() = 0;
            virtual void Maximise() const = 0;
            virtual void Minimise() const = 0;
            virtual void Hide() const = 0;

            virtual float GetScale() const = 0;

            virtual void SwapBuffer() const = 0;

            virtual void Update() const = 0;
        };
    }
}
