#pragma once
#include "RUBENginePCH.h"

namespace RUBENgine
{
	class BaseComponent;
	class GameObject final
	{
		friend class Scene;
	public:
		void Update(const float deltaTime);
		void FixedUpdate(const float fixedDeltaTime);
		void Render() const;
		void AddComponent(BaseComponent* pComponent);
		template <class T>
		T* GetComponent() const;

		void SetActive(const bool isActive) { m_IsActive = isActive; }
		bool IsActive() const { return m_IsActive; }

		GameObject();
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

	private:
		std::vector<BaseComponent*> m_Components;
		bool m_IsActive;
	};
}
