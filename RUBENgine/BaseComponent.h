#pragma once
namespace RUBENgine
{
	class BaseComponent
	{
		friend class GameObject;
	public:
		virtual void Update(const float deltaTime) = 0;
		virtual void FixedUpdate(const float fixedDeltaTime) = 0;
		virtual void Render() const = 0;
		virtual ~BaseComponent() = default;
	protected:
		GameObject* m_pGameObject = nullptr;
	};
}