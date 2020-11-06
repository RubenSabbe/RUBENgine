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
        Down = 0,
        Pressed = 1,
        Released = 2
    };

    struct InputAction
    {
        InputAction(const std::vector<int>& keyCodes, const ButtonState triggerState = ButtonState::Down) : 
            TriggerState(triggerState),
            KeyCodes(keyCodes),
            IsTriggered(false)
        {
        }

        ButtonState TriggerState;
        std::vector<int> KeyCodes;
        bool IsTriggered;
    };
#pragma endregion

    //Checking for all buttons seperately is always slightly faster but for convenience of linking buttons the InputAction exists
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
        bool IsKeyPressed(const int key);
        bool IsKeyReleased(const int key);
        bool IsKeyState(const int key, const ButtonState state);

    private:
        std::map<int, InputAction> m_InputActions;

        BYTE* m_pKeyBoardStateBuffers[2];
        bool m_IsKeyBoardBufferZeroActive;
        POINT m_CurrentMousePosition, m_OldMousePosition, m_MouseMovement;
        bool m_IsInitialized, m_ForceToCenter;

        void UpdateKeyBoardStates();
	};
}

