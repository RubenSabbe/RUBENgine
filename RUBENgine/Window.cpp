#include "Window.h"
#include "GameContext.h"

RUBENgine::Window::Window(HINSTANCE hInstance, const LPCWSTR& windowName, const int windowWidth, const int windowHeight) :
    m_WindowName{ windowName },
    m_WindowWidth{ windowWidth },
    m_WindowHeight{ windowHeight },
    m_WindowHandle{ nullptr },
    m_RTVDescriptorSize{ 0 },
    m_FenceEvent{ nullptr },
    m_FenceValue{ 0 },
    m_IsTearingSupported{ CheckTearingSupport() }
{
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

    if (!RegisterClassExW(&wc))
    {
        MessageBox(NULL, L"Error registering class",
            L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    m_WindowHandle = CreateWindowExW(NULL,
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

    CreateSwapChain();
    CreateDescriptorHeap();
    m_RTVDescriptorSize = GameContext::GetInstance()->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    UpdateRenderTargetViews();

    CreateCommandAllocator();
    CreateCommandList();

    CreateFence();
    CreateEventHandle();

    ShowWindow(m_WindowHandle, true);
    UpdateWindow(m_WindowHandle);
}

RUBENgine::Window::~Window()
{
    Flush();
    CloseHandle(m_FenceEvent);
}

void RUBENgine::Window::Render()
{
    UINT currBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = m_CommandAllocators[currBackBufferIndex];
    Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer = m_BackBuffers[currBackBufferIndex];

    commandAllocator->Reset();
    m_CommandList->Reset(commandAllocator.Get(), nullptr);
    // Clear the render target.
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer.Get(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        m_CommandList->ResourceBarrier(1, &barrier);

        FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
            currBackBufferIndex, m_RTVDescriptorSize);

        m_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
    }

    // Present
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer.Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_CommandList->ResourceBarrier(1, &barrier);

        ThrowIfFailed(m_CommandList->Close());

        ID3D12CommandList* const commandLists[] = {
            m_CommandList.Get()
        };
        GameContext::GetInstance()->pCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

        UINT syncInterval = m_IsVSync ? 1 : 0;
        UINT presentFlags = m_IsTearingSupported && !m_IsVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
        ThrowIfFailed(m_SwapChain->Present(syncInterval, presentFlags));

        m_FrameFenceValues[currBackBufferIndex] = Signal();
        currBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

        WaitForFenceValue(m_FrameFenceValues[currBackBufferIndex]);
    }
}

LRESULT RUBENgine::Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY: // x button on top right corner of window was pressed
        PostQuitMessage(0);
        return 0;
    default:
        return ::DefWindowProcW(hwnd, message, wParam, lParam);
    }
}

void RUBENgine::Window::CreateSwapChain()
{
    //See also: https://www.3dgep.com/learning-directx-12-1/
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory4;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = m_WindowWidth;
    swapChainDesc.Height = m_WindowHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc = { 1, 0 };
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = m_BufferCount;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    // It is recommended to always allow tearing if tearing support is available.
    swapChainDesc.Flags = m_IsTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
    ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
        GameContext::GetInstance()->pCommandQueue.Get(),
        m_WindowHandle,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1));

    // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
    // will be handled manually.
    ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(m_WindowHandle, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain1.As(&m_SwapChain));
}

bool RUBENgine::Window::CheckTearingSupport()
{
    //See also: https://www.3dgep.com/learning-directx-12-1/
    BOOL allowTearing = FALSE;

    // Rather than create the DXGI 1.5 factory interface directly, we create the
    // DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
    // graphics debugging tools which will not support the 1.5 factory interface 
    // until a future update.
    Microsoft::WRL::ComPtr<IDXGIFactory4> factory4;
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
    {
        Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
        if (SUCCEEDED(factory4.As(&factory5)))
        {
            if (FAILED(factory5->CheckFeatureSupport(
                DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                &allowTearing, sizeof(allowTearing))))
            {
                allowTearing = FALSE;
            }
        }
    }

    return allowTearing == TRUE;
}

void RUBENgine::Window::CreateDescriptorHeap()
{
    //See also: https://www.3dgep.com/learning-directx-12-1/
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = m_BufferCount;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    ThrowIfFailed(GameContext::GetInstance()->pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_RTVDescriptorHeap)));
}

void RUBENgine::Window::UpdateRenderTargetViews()
{
    //See also: https://www.3dgep.com/learning-directx-12-1/
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    for (int i = 0; i < m_BufferCount; ++i)
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
        ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

        GameContext::GetInstance()->pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

        m_BackBuffers[i] = backBuffer;

        rtvHandle.Offset(m_RTVDescriptorSize);
    }
}

void RUBENgine::Window::CreateCommandAllocator()
{
    //See also: https://www.3dgep.com/learning-directx-12-1/
    for (int i = 0; i < m_BufferCount; ++i)
    {
        ThrowIfFailed(GameContext::GetInstance()->pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocators[i])));
    }
}

void RUBENgine::Window::CreateCommandList()
{
    //See also: https://www.3dgep.com/learning-directx-12-1/
    ThrowIfFailed(GameContext::GetInstance()->pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocators[m_SwapChain->GetCurrentBackBufferIndex()].Get(), nullptr, IID_PPV_ARGS(&m_CommandList)));

    ThrowIfFailed(m_CommandList->Close());
}

void RUBENgine::Window::CreateFence()
{
    ThrowIfFailed(GameContext::GetInstance()->pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
}

void RUBENgine::Window::CreateEventHandle()
{
    m_FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(m_FenceEvent && "Failed to create fence event.");
}

uint64_t RUBENgine::Window::Signal()
{
    uint64_t fenceValueForSignal = ++m_FenceValue;
    ThrowIfFailed(GameContext::GetInstance()->pCommandQueue->Signal(m_Fence.Get(), fenceValueForSignal));

    return fenceValueForSignal;
}

void RUBENgine::Window::Flush()
{
    WaitForFenceValue(Signal());
}

void RUBENgine::Window::WaitForFenceValue(uint64_t fenceValue)
{
    if (m_Fence->GetCompletedValue() < fenceValue)
    {
        ThrowIfFailed(m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent));
        ::WaitForSingleObject(m_FenceEvent, 0xffffffffUL);
    }
}