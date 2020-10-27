#include "SceneManager.h"
#include "Scene.h"
#include <algorithm>

void RUBENgine::SceneManager::Update(const float deltaTime)
{
	if (m_ActiveScene >= 0)
		m_Scenes[m_ActiveScene]->Update(deltaTime);
}

void RUBENgine::SceneManager::FixedUpdate(const float fixedDeltaTime)
{
	if (m_ActiveScene >= 0)
		m_Scenes[m_ActiveScene]->FixedUpdate(fixedDeltaTime);
}

void RUBENgine::SceneManager::Render() const
{
	if (m_ActiveScene >= 0)
		m_Scenes[m_ActiveScene]->Render();
}

void RUBENgine::SceneManager::AddScene(Scene* pScene)
{
	if (std::find(m_Scenes.begin(), m_Scenes.end(), pScene) == m_Scenes.end())
	{
		m_Scenes.push_back(pScene);
		if (m_ActiveScene == -1)
			m_ActiveScene = 0;
		return;
	}
	//TODO: Replace with logger once I write one (won't have a command window so cout not possible)
	std::cout << "Scene does already exist in the SceneManager\n";
}

void RUBENgine::SceneManager::RemoveScene(const std::string& name)
{
	const std::vector<Scene*>::iterator it = std::find_if(m_Scenes.begin(), m_Scenes.end(), [name](Scene* scene) {return scene->m_Name == name; });
	if (it != m_Scenes.end())
	{
		//Make sure you don't delete the current active scene
		if (it - m_Scenes.begin() == m_ActiveScene)
		{
			//TODO: Replace with logger once I write one (won't have a command window so cout not possible)
			std::cout << "Scene with name '" + name + "' is currently active\n";
			return;
		}
		//Move Index of ActiveScene if earlier scene gets deleted
		else if (it - m_Scenes.begin() < m_ActiveScene)
			m_ActiveScene--;
		//Delete old Scene
		Scene* scene = *it;
		Safe_Delete(scene);
		m_Scenes.erase(it);
		return;
	}
	//TODO: Replace with logger once I write one (won't have a command window so cout not possible)
	std::cout << "Scene with name '" + name + "' does not exist\n";
	return;
}

void RUBENgine::SceneManager::SetSceneActive(const std::string& name)
{
	//Find Scene by Name & Set ActiveIndex to this one
	const std::vector<Scene*>::iterator it = std::find_if(m_Scenes.begin(), m_Scenes.end(), [name](Scene* scene) {return scene->m_Name == name; });
	if (it != m_Scenes.end())
	{
		m_ActiveScene = int(it - m_Scenes.begin());
		return;
	}
	//TODO: Replace with logger once I write one (won't have a command window so cout not possible)
	std::cout << "Scene with name '" + name + "' does not exist\n";
}

RUBENgine::SceneManager::~SceneManager()
{
	for (Scene* pScene : m_Scenes)
	{
		Safe_Delete(pScene);
	}
}
