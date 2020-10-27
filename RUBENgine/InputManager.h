#pragma once
#include "RUBENginePCH.h"
#include "Singleton.h"
#include <Xinput.h>
#include <map>

namespace RUBENgine
{
#pragma region Helpers
    enum class ButtonState
    {
        //DO NOT CHANGE ORDER OR InputManager::Update WILL NO LONGER WORK!!
        Released = 0,
        Pressed = 1,
        Down = 2
    };

    struct InputAction
    {
        InputAction(int keyCode = -1, ButtonState triggerState = ButtonState::Down) : 
            TriggerState(triggerState),
            KeyCode(keyCode),
            IsTriggered(false)
        {
        }

        ButtonState TriggerState;
        int KeyCode;
        bool IsTriggered;
    };
#pragma endregion

	class InputManager final : public Singleton<InputManager>
	{
    public:
        InputManager();
        InputManager(const InputManager& other) = delete;
        InputManager(InputManager&& other) noexcept = delete;
        InputManager& operator=(const InputManager& other) = delete;
        InputManager& operator=(InputManager&& other) noexcept = delete;
        ~InputManager();

        void Init();
        void Update();
        void AddInputAction(const InputAction& action, const int actionID);
        bool IsActionHit(const int actionID) const;
        void SetForceMouseToCenter(const bool forceMouseToCenter);

        POINT GetMousePosition(const bool isOldState = false) const { return (isOldState) ? m_OldMousePosition : m_CurrentMousePosition; }
        POINT GetMouseMovement() const { return m_MouseMovement; }

        void SetCursorVisible(const bool isVisible) { ShowCursor(isVisible); }
        bool IsKeyDown(const int key, const bool isOldState = false);

    private:
        std::map<int, InputAction> m_InputActions;

        BYTE* m_pKeyBoardStateBuffers[2];
        bool m_IsKeyBoardBufferZeroActive;
        POINT m_CurrentMousePosition, m_OldMousePosition, m_MouseMovement;
        bool m_IsInitialized, m_ForceToCenter;

        void UpdateKeyBoardStates();
	};
}

