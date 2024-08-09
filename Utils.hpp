#pragma once
#include "pch.h"

namespace moonlight_xbox_dx {
	namespace Utils {
		struct StreamingStats {
			double averageRenderingTime = 0;
			int queueSize = 0;
			int fps = 0;
			double _accumulatedSeconds = 0;
			int _framesDecoded = 0;
			float outputW = 0;
			float outputH = 0;
			float compositionScaleX = 0;
			float compositionScaleY = 0;
			float compositionScaleMultiplier = 0;
			double renderMs = 0;
			double extraRenderMs = 0;
			double renderDecodeLoopMs = 0; // Average time to run the main loop
			double renderToRenderMs = 0; // Average time between rendering frames (actual displayed FPS)
			double presentTime = 0; // Average time to present a frame
			double fullRenderMs = 0; // Time to run Render with a valid frame (nothing skipped)
			double fullRenderExtraRenderTimeMs = 0;
			double renderToRenderSansPresentTimeMs = 0;

			double waitForNextFrameMs = 0;
			double decodeMs = 0;
			double receiveFrameMs = 0;
			double overheadMs = 0;

			double waitForNextFrameSinceLastRenderMs = 0;
			double decodeSinceLastRenderMs = 0;
			double receiveFrameSinceLastRenderMs = 0;

			double waitForNextFrameLoopMs = 0;
			double decodeLoopMs = 0;
			double receiveFrameLoopMs = 0;

			void LoopStarted();
			void GotPacket();
			void GotFrame();
			void FrameRendered();
			void LoopEnded();

		private:
			bool _frameRenderedThisLoop = false;
			bool _gotPacketThisLoop = false;
			bool _gotFrameThisLoop = false;
			LARGE_INTEGER _loopStart;
			double accumulatedRenderToRenderMs = 0;
		};

		enum class TimerType { RunningAverage, Accumulate };

		template<TimerType T = TimerType::RunningAverage>
		class ScopedStatTimer {
			ScopedStatTimer() = delete;
			ScopedStatTimer(const ScopedStatTimer&) = delete;
			ScopedStatTimer& operator=(const ScopedStatTimer&) = delete;

			public:
				ScopedStatTimer(double& statValue, double* accumulator = nullptr) : m_statValue(statValue), m_accumulator(accumulator) {
					QueryPerformanceCounter(&m_start);
				}

				~ScopedStatTimer() {
					LARGE_INTEGER end, frequency;
					QueryPerformanceCounter(&end);
					QueryPerformanceFrequency(&frequency);
					double ms = 1000. * (end.QuadPart - m_start.QuadPart) / (float)(frequency.QuadPart);

					m_statValue = 0.5 * ms + 0.5 * m_statValue;

					if (m_accumulator != nullptr)
					{
						*m_accumulator += ms;
					}
				}

		private:
				double& m_statValue;
				double* m_accumulator;
				LARGE_INTEGER m_start;
		};

		template<>
		ScopedStatTimer<TimerType::Accumulate>::~ScopedStatTimer() {
			LARGE_INTEGER end, frequency;
			QueryPerformanceCounter(&end);
			QueryPerformanceFrequency(&frequency);
			double ms = 1000. * (end.QuadPart - m_start.QuadPart) / (float)(frequency.QuadPart);

			m_statValue += ms;
		}
		
		extern std::vector<std::wstring> logLines;
		extern bool showLogs;
		extern bool showStats;
		extern float outputW;
		extern float outputH;
		extern StreamingStats stats;
		extern std::mutex logMutex;
		
		Platform::String^ StringPrintf(const char* fmt, ...);

		void Log(const char* fmt);

		std::vector<std::wstring> GetLogLines();
		Platform::String^ StringFromChars(char* chars);
		Platform::String^ StringFromStdString(std::string st);
		std::string PlatformStringToStdString(Platform::String^ input);

	}
}