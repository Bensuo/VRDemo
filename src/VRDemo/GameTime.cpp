#include "GameTime.h"
#include <thread>
#include <iostream>

namespace Engine
{
	namespace Time
	{
		GameTime::GameTime()
		{
			m_startTime = std::chrono::high_resolution_clock::now();

			m_frames = 0;
			m_frameCap = MAX_FRAMERATE;
			m_frameCounter = std::chrono::duration<float>(0);
			m_deltaTime = std::chrono::duration<float>(1.0 / m_frameCap);
			m_unprocessedTime = std::chrono::duration<float>(0);
		}

		double GameTime::GetTime() const
		{
			//find current time in miliseconds, return this data in terms of seconds
			const auto current_time = std::chrono::high_resolution_clock::now();
			const auto value = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - m_startTime).count();
			return value / 1000.0;
		}

		std::chrono::steady_clock::time_point GameTime::GetTimePoint()
		{
			return std::chrono::steady_clock::now();
		}

		const float& GameTime::GetDeltaTime() const
		{
			return static_cast<float>(m_deltaTime.count());
		}

		void GameTime::SetFrameCap(const unsigned& frameCap)
		{
			this->m_frameCap = frameCap;
		}

		void GameTime::Initialize()
		{
			m_lastTime = GetTimePoint();
		}

		void GameTime::Reset()
		{
			//calculate current frame duration
			const auto startTime = GetTimePoint();
			const auto passedTime = startTime - m_lastTime;
			m_lastTime = startTime;
			m_unprocessedTime += passedTime;
			m_frameCounter += passedTime;

			//display FPS m_Counter
			if (m_frameCounter >= std::chrono::duration<double>(1))
			{
				std::cout << m_frames << "\n";
				m_frames = 0;
				m_frameCounter = std::chrono::duration<double>(0);
			}
		}

		bool GameTime::FrameComplete() const
		{
			return m_unprocessedTime < m_deltaTime;
		}

		void GameTime::Update()
		{
			m_unprocessedTime -= m_deltaTime;
		}

		void GameTime::IncrementFrames()
		{
			m_frames++;
		}

		void GameTime::Sleep()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}
