#pragma once
#include "RUBENginePCH.h"
#include <functional>
#include <map>

namespace RUBENgine
{
	class GameObject;
	class Scene final
	{
		friend class SceneManager;

	public:
		void AddGameObject(GameObject* pObject);
		void RemoveObject(GameObject* pObject);
		void Invoke(std::function<void()> func, float delay);

		Scene(const std::string& name);
		~Scene();
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		void Update(const float);
		void FixedUpdate(const float);
		void Render() const;

	private:
		std::string m_Name;
		std::vector<GameObject*> m_Objects{};
		std::vector<std::pair<std::function<void()>, float>> m_Invokers;
	};

}
