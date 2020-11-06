#include "Application.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Window.h"
#include "Logger.h"

const float RUBENgine::Application::m_SecPerFrame = 0.016f;

void RUBENgine::Application::Initialize(HINSTANCE hInstance, const LPCWSTR& windowTitle, const int windowWidth, const int windowHeight, const bool useWarp)
{
	m_UseWarp = useWarp;
	EnableDebugLayer();
	GameContext::GetInstance()->pDevice = GetDevice();
	//Command Queue Uses the Device so DO NOT swap order
	GameContext::GetInstance()->pCommandQueue = GetCommandQueue();
	//Window Uses the Command Queue so DO NOT swap order
	GameContext::GetInstance()->pWindow = new Window(hInstance, windowTitle, windowWidth, windowHeight);
	InputManager::GetInstance()->Init();
	InputManager::GetInstance()->AddInputAction(InputAction({ 'A' }, ButtonState::Released), 1);
}

void RUBENgine::Application::Cleanup()
{
	SceneManager::Release();
	InputManager::Release();
	GameContext::Release();
	Logger::Release();
}

void RUBENgine::Application::Run()
{
	bool doContinue = true;
	float lag = 0.0f;
	auto lastTime = std::chrono::high_resolution_clock::now();
	while (doContinue)
	{
		const auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
		lastTime = currentTime;
		lag += deltaTime;
		SceneManager::GetInstance()->Update(deltaTime);
		while (lag >= m_SecPerFrame)
		{
			InputManager::GetInstance()->Update();
			if (InputManager::GetInstance()->IsKeyDown(VK_ESCAPE))
				doContinue = false;
			if (InputManager::GetInstance()->IsActionHit(1))
				Logger::GetInstance()->LogInfo(L"Test");
			SceneManager::GetInstance()->FixedUpdate(m_SecPerFrame);
			lag -= m_SecPerFrame;
		}
		SceneManager::GetInstance()->Render();
		GameContext::GetInstance()->pWindow->Render();
	}
	
	Cleanup();
}

Microsoft::WRL::ComPtr<IDXGIAdapter4> RUBENgine::Application::GetAdapter()
{
	//See also: https://www.3dgep.com/learning-directx-12-1/
	Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

	Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgiAdapter1;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter4;

	if (m_UseWarp)
	{
		ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
		ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
	}
	else
	{
		SIZE_T maxDedicatedVideoMemory = 0;
		for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
			dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

			//Check to see if the adapter can create a D3D12 device without actually 
			//creating it. The adapter with the largest dedicated video memory
			//is favored.
			if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
				SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
					D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
				dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
			{
				maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
				ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
			}
		}
	}

	return dxgiAdapter4;
}

Microsoft::WRL::ComPtr<ID3D12Device2> RUBENgine::Application::GetDevice()
{
	//See also: https://www.3dgep.com/learning-directx-12-1/
	Microsoft::WRL::ComPtr<ID3D12Device2> d3d12Device2;
	ThrowIfFailed(D3D12CreateDevice(GetAdapter().Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device2)));

	//Enable debug messages in debug mode.
#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue;
	if (SUCCEEDED(d3d12Device2.As(&pInfoQueue)))
	{
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

		//Suppress messages based on their severity level
		D3D12_MESSAGE_SEVERITY Severities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO
		};

		//Suppress individual messages by their ID
		D3D12_MESSAGE_ID DenyIds[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   //Not sure how to avoid this message.
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         //Warning occurs when using capture frame while graphics debugging.
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       //Warning occurs when using capture frame while graphics debugging.
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		NewFilter.DenyList.NumSeverities = _countof(Severities);
		NewFilter.DenyList.pSeverityList = Severities;
		NewFilter.DenyList.NumIDs = _countof(DenyIds);
		NewFilter.DenyList.pIDList = DenyIds;

		ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
	}
#endif

	return d3d12Device2;
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> RUBENgine::Application::GetCommandQueue()
{
	//See also: https://www.3dgep.com/learning-directx-12-1/
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	ThrowIfFailed(GameContext::GetInstance()->pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue)));

	return d3d12CommandQueue;
}

void RUBENgine::Application::EnableDebugLayer()
{
#if defined(_DEBUG)
	// Always enable the debug layer before doing anything DX12 related
	// so all possible errors generated while creating DX12 objects
	// are caught by the debug layer.
	Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
	debugInterface->EnableDebugLayer();
#endif
}
