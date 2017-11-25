#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <chrono>

namespace Engine
{
    using GameTime = float;

    namespace Time
    {
        /**
         * \brief A basic timer. Used for the game loop.
         */
        class GameTimer
        {
            std::chrono::time_point<std::chrono::steady_clock> start_time;
            std::chrono::time_point<std::chrono::steady_clock> last_time;

            unsigned frames = 0;
            unsigned frame_cap;
            const unsigned max_framerate = 90;

            std::chrono::duration<double> frame_counter;
            std::chrono::duration<double> delta_time;
            std::chrono::duration<double> unprocessed_time;

        public:
            GameTimer();
            GameTime Time() const;
            static std::chrono::steady_clock::time_point GetTimePoint();
            GameTime DeltaTime() const;

            void SetFrameCap(const unsigned frame_cap);
            void Initialize();
            void Reset();
            bool FrameComplete() const;
            void Update();
            void IncrementFrames();
            static void Sleep();
        };
    }
}
#endif // GAMETIMER_H
