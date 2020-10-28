#pragma once

#include <stdio.h>
#include <iostream> // std::cout
#include <sstream> // stringstream
#include <memory> // smart pointers
#include <vector>
#include "d3dx12.h"
#include <chrono>
#include <thread>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

namespace RUBENgine
{
	template<class T>
	void Safe_Delete(T& toDelete)
	{
		if (toDelete)
		{
			delete toDelete;
			toDelete = nullptr;
		}
	}
}