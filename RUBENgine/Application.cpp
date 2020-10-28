#include "Application.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Window.h"

const float RUBENgine::Application::m_SecPerFrame = 0.016f;

void RUBENgine::Application::Initialize(HINSTANCE hInstance, const LPCWSTR& windowTitle, const int windowWidth, const int windowHeight)
{
	UNREFERENCED_PARAMETER(windowTitle);
	UNREFERENCED_PARAMETER(windowWidth);
	UNREFERENCED_PARAMETER(windowHeight);
	Window::GetInstance()->Initialize(m_HWnd, hInstance, windowTitle, windowWidth, windowHeight);
	//if (SDL_Init(SDL_INIT_VIDEO) != 0)
	//{
	//	throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	//}
	//
	//m_pWindow = SDL_CreateWindow(
	//	windowTitle,
	//	SDL_WINDOWPOS_UNDEFINED,
	//	SDL_WINDOWPOS_UNDEFINED,
	//	windowWidth,
	//	windowHeight,
	//	SDL_WINDOW_OPENGL
	//);
	//if (m_pWindow == nullptr)
	//{
	//	throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	//}
	//Renderer::GetInstance()->Init(m_pWindow, { 0, 0, 0, 255 });
	//Logger::Init();
	InputManager::GetInstance()->Init();
	//ResourceManager::GetInstance()->Init(dataPath + "Images/");
	//SoundManager::GetInstance()->Init(dataPath + "Sound/");
	//PhysicsManager::GetInstance()->Init({ 0.0f, -9.81f }, 8, 3, ppm, { 0, 255, 0, 255 });
}

void RUBENgine::Application::Cleanup()
{
	//Renderer::GetInstance()->Destroy();
	//SDL_DestroyWindow(m_pWindow);
	//Renderer::Release();
	SceneManager::Release();
	InputManager::Release();
	Window::Release();
	//ResourceManager::Release();
	//SoundManager::Release();
	//PhysicsManager::Release();
	//m_pWindow = nullptr;
	//SDL_Quit();
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
			SceneManager::GetInstance()->FixedUpdate(m_SecPerFrame);
			lag -= m_SecPerFrame;
		}
		SceneManager::GetInstance()->Render();
	}
	
	Cleanup();
}