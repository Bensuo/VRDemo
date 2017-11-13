#include "GameTimer.h"
#include <thread>
#include <iostream>

namespace Engine
{
    namespace Time
    {
        GameTimer::GameTimer()
        {
            start_time = std::chrono::high_resolution_clock::now();

            frames = 0;
            frame_cap = max_framerate;
            frame_counter = std::chrono::duration<float>(0);
            delta_time = std::chrono::duration<float>(1.0 / frame_cap);
            unprocessed_time = std::chrono::duration<float>(0);
        }

        GameTime GameTimer::Time() const
        {
            //find current time in miliseconds, return this data in terms of seconds
            const auto current_time = std::chrono::high_resolution_clock::now();
            const auto value = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
            return value / 1000.0f;
        }

        std::chrono::steady_clock::time_point GameTimer::GetTimePoint()
        {
            return std::chrono::steady_clock::now();
        }

        GameTime GameTimer::DeltaTime() const
        {
            return static_cast<GameTime>(delta_time.count());
        }

        void GameTimer::SetFrameCap(const unsigned frame_cap)
        {
            this->frame_cap = frame_cap;
        }

        void GameTimer::Initialize()
        {
            last_time = GetTimePoint();
        }

        void GameTimer::Reset()
        {
            //calculate current frame duration
            const auto start_time = GetTimePoint();
            const auto passed_time = start_time - last_time;
            last_time = start_time;
            unprocessed_time += passed_time;
            frame_counter += passed_time;

            //display FPS Counter
            if (frame_counter >= std::chrono::duration<double>(1))
            {
                std::cout << frames << "\n";
                frames = 0;
                frame_counter = std::chrono::duration<double>(0);
            }
        }

        bool GameTimer::FrameComplete() const
        {
            return unprocessed_time < delta_time;
        }

        void GameTimer::Update()
        {
            unprocessed_time -= delta_time;
        }

        void GameTimer::IncrementFrames()
        {
            frames++;
        }

        void GameTimer::Sleep()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
