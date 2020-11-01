// TestApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Application.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
    RUBENgine::Application temp;
    temp.Initialize(hInstance, L"Test", 1080, 720);
    temp.Run();

    return 0;
}