#pragma once
#include <string>
#include "Singleton.h"
namespace RUBENgine
{
	class Logger final : public Singleton<Logger>
	{
	public:
		Logger();
		~Logger();

		void LogInfo(const std::wstring& text, bool includeTimeStamp = false);
		void LogWarning(const std::wstring& text, bool includeTimeStamp = false);
		void LogError(const std::wstring& text, bool includeTimeStamp = false);
		void Log(const std::wstring& text, const std::wstring& label, bool includeTimeStamp = false);

	private:
		std::wstring GetTimeStamp();
	};
}