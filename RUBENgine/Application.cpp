#include "Application.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Window.h"
#include "Logger.h"

const float RUBENgine::Application::m_SecPerFrame = 0.016f;

void RUBENgine::Application::Initialize(HINSTANCE hInstance, const LPCWSTR& windowTitle, const int windowWidth, const int windowHeight)
{
	UNREFERENCED_PARAMETER(windowTitle);
	UNREFERENCED_PARAMETER(windowWidth);
	UNREFERENCED_PARAMETER(windowHeight);
	Window::GetInstance()->Initialize(hInstance, windowTitle, windowWidth, windowHeight);
	InputManager::GetInstance()->Init();
	//Renderer::GetInstance()->Init(m_pWindow, { 0, 0, 0, 255 });
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
	Logger::Release();
	//ResourceManager::Release();
	//SoundManager::Release();
	//PhysicsManager::Release();
	//m_pWindow = nullptr;
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