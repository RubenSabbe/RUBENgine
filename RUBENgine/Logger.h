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

	private:
		std::wstring GetTimeStamp();
	};
}

