#pragma once
#include "pch.h"
#include "Utils.hpp"
#define LOG_LINES 64

namespace moonlight_xbox_dx {
	namespace Utils {
		void StreamingStats::LoopStarted() {
			QueryPerformanceCounter(&_loopStart);
			_gotPacketThisLoop = false;
			_gotFrameThisLoop = false;
			_frameRenderedThisLoop = false;
		}

		void StreamingStats::GotPacket() {
			_gotPacketThisLoop = true;
		}

		void StreamingStats::GotFrame() {
			_gotFrameThisLoop = true;
		}

		void StreamingStats::FrameRendered() {
			_frameRenderedThisLoop = true;
		}

		void StreamingStats::LoopEnded() {
			LARGE_INTEGER end, frequency;
			QueryPerformanceCounter(&end);
			QueryPerformanceFrequency(&frequency);
			double ms = 1000. * (end.QuadPart - _loopStart.QuadPart) / (float)(frequency.QuadPart);

			accumulatedRenderToRenderMs += ms;

			if (_frameRenderedThisLoop) {
				renderToRenderMs = 0.5 * accumulatedRenderToRenderMs + 0.5 * renderToRenderMs;
				renderToRenderSansPresentTimeMs = renderToRenderMs - presentTime;
				accumulatedRenderToRenderMs = 0;
				overheadMs = renderToRenderMs - waitForNextFrameMs - decodeMs - receiveFrameMs - presentTime;
				fullRenderMs = 0.5 * renderMs + 0.5 * fullRenderMs;
				fullRenderExtraRenderTimeMs = 0.5 * extraRenderMs + 0.5 * fullRenderExtraRenderTimeMs;
			}
		}

		std::vector<std::wstring> logLines;
		bool showLogs = false;
		bool showStats = false;
		StreamingStats stats;
		std::mutex logMutex;

		Platform::String^ StringPrintf(const char* fmt, ...) {
			va_list list;
			va_start(list, fmt);
			char message[2048];
			vsprintf_s(message, 2047, fmt, list);
			std::string s_str = std::string(message);
			std::wstring wid_str = std::wstring(s_str.begin(), s_str.end());
			const wchar_t* w_char = wid_str.c_str();
			Platform::String^ p_string = ref new Platform::String(w_char);
			return p_string;
		}

		void Log(const char* fmt) {
			try {
				if (fmt == nullptr) return;
				size_t len = strlen(fmt) + 1;
				wchar_t* stringBuf = (wchar_t*)malloc(sizeof(wchar_t) * len);
				if (stringBuf == NULL)return;
				mbstowcs(stringBuf, fmt, len);
				std::wstring string(stringBuf);
				logMutex.lock();
				if (logLines.size() == LOG_LINES)logLines.erase(logLines.begin());
				logLines.push_back(string);
				logMutex.unlock();
				OutputDebugStringA(fmt);
			}
			catch (...){

			}
		}

		std::vector<std::wstring> GetLogLines() {
			return logLines;
		}

		//https://stackoverflow.com/a/20707518
		Platform::String^ StringFromChars(char* chars)
		{
			int wchars_num = MultiByteToWideChar(CP_UTF8, 0, chars, -1, NULL, 0);
			wchar_t* wstr = new wchar_t[wchars_num];
			MultiByteToWideChar(CP_UTF8, 0, chars, -1, wstr, wchars_num);
			Platform::String^ str = ref new Platform::String(wstr);
			delete[] wstr;
			return str;
		}

		//https://stackoverflow.com/a/43628199
		Platform::String^ StringFromStdString(std::string input) {
			std::wstring w_str = std::wstring(input.begin(), input.end());
			const wchar_t* w_chars = w_str.c_str();
			return (ref new Platform::String(w_chars));
		}

		//https://stackoverflow.com/a/35905753
		std::string PlatformStringToStdString(Platform::String ^input) {
			std::wstring fooW(input->Begin());
			std::string fooA(fooW.begin(), fooW.end());
			return fooA;
		}
	}

}