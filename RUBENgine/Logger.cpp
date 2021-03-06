#include "Logger.h"
#include "RUBENginePCH.h"

RUBENgine::Logger::Logger() {}

RUBENgine::Logger::~Logger() {}

void RUBENgine::Logger::LogInfo(const std::wstring& text, bool includeTimeStamp)
{
	Log(text, L"INFO", includeTimeStamp);
}

void RUBENgine::Logger::LogWarning(const std::wstring& text, bool includeTimeStamp)
{
	Log(text, L"WARNING", includeTimeStamp);
}

void RUBENgine::Logger::LogError(const std::wstring& text, bool includeTimeStamp)
{
	Log(text, L"ERROR", includeTimeStamp);
}

void RUBENgine::Logger::Log(const std::wstring& text, const std::wstring& label, bool includeTimeStamp)
{
	OutputDebugStringW(((includeTimeStamp ? GetTimeStamp() : L"") + L"[" + label + L"]: " + text + L"\n").c_str());
}

std::wstring RUBENgine::Logger::GetTimeStamp()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	std::stringstream stringStream;
	//Hour + 1 because default is GMT + 0 (I live in GMT + 1)
	stringStream << L"[" << st.wYear << L"-" << st.wMonth << L"-" << st.wDay << L" | " << (st.wHour + 1) % 24 << L":" << st.wMinute << L":" << st.wSecond << L":" << st.wMilliseconds << L"]";
	return std::wstring(stringStream.str().begin(), stringStream.str().end());
}
