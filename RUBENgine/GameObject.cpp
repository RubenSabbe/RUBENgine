#include "RUBENginePCH.h"
#include "GameObject.h"
#include "BaseComponent.h"
#include <exception>

RUBENgine::GameObject::GameObject() :
	m_IsActive(true)
{

}

RUBENgine::GameObject::~GameObject()
{
	for (BaseComponent* pComp : m_Components)
		Safe_Delete(pComp);
}

void RUBENgine::GameObject::Update(const float deltaTime)
{
	if (!m_IsActive)
		return;
	for (BaseComponent* pComp : m_Components)
		pComp->Update(deltaTime);
}

void RUBENgine::GameObject::FixedUpdate(const float fixedDeltaTime)
{
	if (!m_IsActive)
		return;
	for (BaseComponent* pComp : m_Components)
		pComp->FixedUpdate(fixedDeltaTime);
}

void RUBENgine::GameObject::Render() const
{
	if (!m_IsActive)
		return;
	for (BaseComponent* pComp : m_Components)
		pComp->Render();
}

void RUBENgine::GameObject::AddComponent(BaseComponent* pComponent)
{
	//Prevent Duplicate ComponentTypes
	for (BaseComponent* pComp : m_Components)
	{
		if (typeid(*pComp) == typeid(*pComponent))
		{
			//TODO: Replace with logger once I write one (won't have a command window so cout not possible)
			std::cout << "Component already added\n";
			return;
		}
	}
	m_Components.push_back(pComponent);
	pComponent->m_pGameObject = this;
}

template<class T>
T* RUBENgine::GameObject::GetComponent() const
{
	//Find Component of type T and return it if it exists
	const type_info& ti = typeid(T);
	for (BaseComponent* pComp : m_Components)
	{
		if (pComp && typeid(*pComp) == ti)
			return static_cast<T*>(pComp);
	}
	return nullptr;
}
