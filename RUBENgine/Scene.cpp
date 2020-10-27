#include "Scene.h"
#include "GameObject.h"
#include <algorithm>

RUBENgine::Scene::Scene(const std::string& name) : 
	m_Name(name)
{

}

RUBENgine::Scene::~Scene()
{
	for (GameObject* obj : m_Objects)
	{
		Safe_Delete(obj);
	}
	m_Objects.clear();
}

void RUBENgine::Scene::Invoke(std::function<void()>func, float delay)
{
	m_Invokers.push_back({ func, delay });
}

void RUBENgine::Scene::AddGameObject(GameObject* pObject)
{
	m_Objects.push_back(pObject);
}

void RUBENgine::Scene::RemoveObject(GameObject* pObject)
{
	//Remove GameObject & Resize Vector of GameObjects
	for (size_t objCounter{}; objCounter < m_Objects.size(); objCounter++)
	{
		if (m_Objects[objCounter] == pObject)
		{
			Safe_Delete(pObject);
			m_Objects[objCounter] = m_Objects.back();
			m_Objects.pop_back();
		}
	}
}

void RUBENgine::Scene::Update(const float deltaTime)
{
	for (GameObject* obj : m_Objects)
		obj->Update(deltaTime);
}

void RUBENgine::Scene::FixedUpdate(const float fixedDeltaTime)
{
	//Update All Invoke Timers && Execute + Remove from vector if Timer reaches 0.0f
	for (size_t invokeCounter{}; invokeCounter < m_Invokers.size(); invokeCounter++)
	{
		m_Invokers[invokeCounter].second -= fixedDeltaTime;
		if (m_Invokers[invokeCounter].second < 0.0f)
		{
			m_Invokers[invokeCounter].first();
			m_Invokers[invokeCounter] = m_Invokers.back();
			m_Invokers.pop_back();
			invokeCounter--;
		}
	}

	for (GameObject* obj : m_Objects)
	{
		obj->FixedUpdate(fixedDeltaTime);
	}
}

void RUBENgine::Scene::Render() const
{
	for (const GameObject* obj : m_Objects)
		obj->Render();
}

