#include "Window.h"

LRESULT RUBENgine::Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY: // x button on top right corner of window was pressed
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

RUBENgine::Window::Window() :
    m_WindowWidth{ 0 },
    m_WindowHeight{ 0 },
    m_WindowName{ L"" }
{
}

RUBENgine::Window::~Window()
{
}

void RUBENgine::Window::Initialize(HINSTANCE hInstance, const LPCWSTR& windowName, const int windowWidth, const int windowHeight)
{
    const_cast<LPCWSTR&>(m_WindowName) = windowName;
    const_cast<int&>(m_WindowWidth) = windowWidth;
    const_cast<int&>(m_WindowHeight) = windowHeight;

    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = RUBENgine::Window::WndProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = windowName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Error registering class",
            L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    m_WindowHandle = CreateWindowEx(NULL,
        windowName,
        windowName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowWidth, windowHeight,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!m_WindowHandle)
    {
        MessageBox(NULL, L"Error creating window",
            L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    ShowWindow(m_WindowHandle, true);
    UpdateWindow(m_WindowHandle);
}
