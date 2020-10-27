// TestApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Application.h"

int main()
{
    RUBENgine::Application temp;
    temp.Initialize("Test", 1080, 720);
    temp.Run();
}