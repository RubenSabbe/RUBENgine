#pragma once
#include "RUBENginePCH.h"
#include "Singleton.h"

namespace RUBENgine
{
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		SceneManager() = default;
		void AddScene(Scene* pScene);
		void RemoveScene(const std::string& name);
		void SetSceneActive(const std::string& name);
		Scene* GetActiveScene() const { return m_Scenes[m_ActiveScene]; };
		~SceneManager();

		void Update(const float deltaTime);
		void FixedUpdate(const float fixedDeltaTime);
		void Render() const;
	private:
		std::vector<Scene*> m_Scenes;
		int m_ActiveScene{ -1 };
	};
}
