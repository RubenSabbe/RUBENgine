#pragma once
#include "Window.h"
#include "Singleton.h"

namespace RUBENgine
{
	class GameContext final : public Singleton<GameContext>
	{
	public:
		Window* pWindow{ nullptr };
		Microsoft::WRL::ComPtr<ID3D12Device2> pDevice;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCommandQueue;
		~GameContext()
		{
			delete pWindow;
		}
	};
}