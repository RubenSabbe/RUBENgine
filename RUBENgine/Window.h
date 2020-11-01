#pragma once
#include "RUBENginePCH.h"

namespace RUBENgine
{
	class Window final
	{
	public:
		Window(HINSTANCE hInstance, const LPCWSTR& windowName, const int windowWidth, const int windowHeight);
		~Window();

		void Render();

		const int GetWindowWidth() const { return m_WindowWidth; }
		const int GetWindowHeight() const { return m_WindowHeight; }
		const LPCWSTR GetWindowName() const { return m_WindowName; }
		HWND GetWindowHandle() const { return m_WindowHandle; }
		const Microsoft::WRL::ComPtr<IDXGISwapChain4>& GetSwapChain() const { return m_SwapChain; }

	private:
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		const int m_WindowWidth, m_WindowHeight;
		const LPCWSTR m_WindowName;
		HWND m_WindowHandle;
		const static uint32_t m_BufferCount{ 3 };
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap;
		UINT m_RTVDescriptorSize;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_BackBuffers[m_BufferCount];
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocators[m_BufferCount];
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
		HANDLE m_FenceEvent;
		uint64_t m_FenceValue;
		uint64_t m_FrameFenceValues[m_BufferCount] = {};

		bool m_IsVSync;
		bool m_IsTearingSupported;

		void CreateSwapChain();
		bool CheckTearingSupport();
		void CreateDescriptorHeap();
		void UpdateRenderTargetViews();
		void CreateCommandAllocator();
		void CreateCommandList();
		void CreateFence();
		void CreateEventHandle();

		uint64_t Signal();
		void Flush();
		void WaitForFenceValue(uint64_t fenceValue);
	};
}

