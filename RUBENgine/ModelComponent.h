#pragma once
#include "BaseComponent.h"
namespace RUBENgine
{
	class ModelComponent final : public BaseComponent
	{
	public:
		void Update(const float deltaTime) override;
		void FixedUpdate(const float fixedDeltaTime) override;
		void Render() const override;
	};
}

