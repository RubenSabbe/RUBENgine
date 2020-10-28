#pragma once
#include "RUBENginePCH.h"
#include "Singleton.h"

namespace RUBENgine
{
	class Window final : public Singleton<Window>
	{
	public:
		Window();
		~Window();
		void Initialize(HINSTANCE hInstance, const LPCWSTR& windowName, const int windowWidth, const int windowHeight);

		const int GetWindowWidth() const { return m_WindowWidth; }
		const int GetWindowHeight() const { return m_WindowHeight; }
		const LPCWSTR GetWindowName() const { return m_WindowName; }
		HWND GetWindowHandle() const { return m_WindowHandle; }

	private:
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		const int m_WindowWidth, m_WindowHeight;
		const LPCWSTR m_WindowName;
		HWND m_WindowHandle;
	};
}

