#pragma once
#include "RUBENginePCH.h"
#include "GameContext.h"

namespace RUBENgine
{
	class Application
	{
	public:
		void Initialize(HINSTANCE hInstance, const LPCWSTR& windowTitle, const int windowWidth, const int windowHeight, bool useWarp = false);
		void Cleanup();
		void Run();

	private:
		static const float m_SecPerFrame; //0.016 for 60 fps, 0.033 for 30 fps
		bool m_UseWarp;

		Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter();
		Microsoft::WRL::ComPtr<ID3D12Device2> GetDevice();
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue();
	};
}