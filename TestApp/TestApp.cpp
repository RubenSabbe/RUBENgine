// TestApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Application.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "ModelComponent.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
    RUBENgine::Application temp;
    temp.Initialize(hInstance, L"Test", 1080, 720);

    RUBENgine::GameObject* gameObject = new RUBENgine::GameObject();
    gameObject->AddComponent(new RUBENgine::ModelComponent());
    RUBENgine::Scene* scene = new RUBENgine::Scene("DefaultScene");
    scene->AddGameObject(gameObject);
    RUBENgine::SceneManager::GetInstance()->AddScene(scene);

    temp.Run();

    return 0;
}