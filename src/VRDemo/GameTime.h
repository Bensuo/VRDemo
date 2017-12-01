#pragma once

#include <chrono>

/*
* Names: Stuart Adams and Ben Tracy
* Student IDs: B00265262 & B00307589
*
* Acknowledgements: Code is from Stuart Adams' individual coursework for Advanced Graphics
*/

namespace Engine
{
	namespace Time
	{
		class GameTime
		{
			std::chrono::time_point<std::chrono::steady_clock> m_startTime;
			std::chrono::time_point<std::chrono::steady_clock> m_lastTime;

			unsigned m_frames = 0;
			unsigned m_frameCap;
			const unsigned MAX_FRAMERATE = 60;

			std::chrono::duration<double> m_frameCounter;
			std::chrono::duration<double> m_deltaTime;
			std::chrono::duration<double> m_unprocessedTime;

		public:
			GameTime();
			double GetTime() const;
			static std::chrono::steady_clock::time_point GetTimePoint();
			const float& GetDeltaTime() const;

			void SetFrameCap(const unsigned& frameCap);
			void Initialize();
			void Reset();
			bool FrameComplete() const;
			void Update();
			void IncrementFrames();
			static void Sleep();
		};
	}
}
