#include "InputManager.h"
#include "GameContext.h"

RUBENgine::InputManager::InputManager() :
	m_InputActions{},
	m_pKeyBoardStateBuffers{ nullptr, nullptr },
	m_IsKeyBoardBufferZeroActive{ true },
	m_CurrentMousePosition{ 0,0 },
	m_OldMousePosition{ 0,0 },
	m_MouseMovement{ 0,0 },
	m_IsInitialized{ false },
	m_ForceToCenter{ false }
{
}

RUBENgine::InputManager::~InputManager()
{
	//Prevent Memory Leaks
	if (m_pKeyBoardStateBuffers[0] != nullptr)
	{
		delete[] m_pKeyBoardStateBuffers[0];
		m_pKeyBoardStateBuffers[0] = nullptr;
	}
	if (m_pKeyBoardStateBuffers[1] != nullptr)
	{
		delete[] m_pKeyBoardStateBuffers[1];
		m_pKeyBoardStateBuffers[1] = nullptr;
	}

	m_IsInitialized = false;
}

void RUBENgine::InputManager::Init()
{
	//Locate Memory for KeyBoard & Mouse states (both in KeyBoardState)
	if (m_IsInitialized)
		return;

	m_pKeyBoardStateBuffers[0] = new BYTE[256];
	m_pKeyBoardStateBuffers[1] = new BYTE[256];

	bool gotKeyboardState = false;
	gotKeyboardState = GetKeyboardState(m_pKeyBoardStateBuffers[0]);
	gotKeyboardState = GetKeyboardState(m_pKeyBoardStateBuffers[1]);

	m_IsInitialized = true;
}

void RUBENgine::InputManager::Update()
{
	UpdateKeyBoardStates();

	//Reset previous InputAction States
	for (auto it = m_InputActions.begin(); it != m_InputActions.end(); ++it)
	{
		auto currAction = &(it->second);

		//Reset the previous state before updating/checking the new state
		currAction->IsTriggered = false;

		for (int keyCode : currAction->KeyCodes)
		{
			//First check/previous frame: will return false if currAction->TriggerState == Pressed||1
			//Second check/current frame: will return false if currAction->TriggerState == Released||2
			if ((int(currAction->TriggerState) & 1) != IsKeyDown(keyCode, true) && bool(int(currAction->TriggerState) & 2) != IsKeyDown(keyCode))
			{
				currAction->IsTriggered = true;
				continue;
			}
		}
	}

	//Mouse Position
	m_OldMousePosition = m_CurrentMousePosition;
	if (GetCursorPos(&m_CurrentMousePosition))
	{
		ScreenToClient(GameContext::GetInstance()->pWindow->GetWindowHandle(), &m_CurrentMousePosition);
	}

	m_MouseMovement.x = m_CurrentMousePosition.x - m_OldMousePosition.x;
	m_MouseMovement.y = m_CurrentMousePosition.y - m_OldMousePosition.y;

	if (m_ForceToCenter)
	{
		POINT mouseCenter;
		m_CurrentMousePosition.x = GameContext::GetInstance()->pWindow->GetWindowWidth() / 2;
		m_CurrentMousePosition.y = GameContext::GetInstance()->pWindow->GetWindowHeight() / 2;
		mouseCenter.x = m_CurrentMousePosition.x;
		mouseCenter.y = m_CurrentMousePosition.y;
		ClientToScreen(GameContext::GetInstance()->pWindow->GetWindowHandle(), &mouseCenter);

		SetCursorPos(mouseCenter.x, mouseCenter.y);
	}
}

void RUBENgine::InputManager::AddInputAction(const InputAction& action, const int actionID)
{
	if (m_InputActions.find(actionID) != m_InputActions.end())
	{
		//TODO: Replace with logger once I write one (won't have a command window so cout not possible)
		std::cout << "This actionID is already being used\n";
		return;
	}
	m_InputActions[actionID] = action;
}

bool RUBENgine::InputManager::IsActionHit(const int actionID) const
{
	if (m_InputActions.find(actionID) == m_InputActions.cend())
		return false;
	return m_InputActions.at(actionID).IsTriggered;
}

void RUBENgine::InputManager::SetForceMouseToCenter(const bool forceMouseToCenter)
{
	m_ForceToCenter = forceMouseToCenter;

	if (forceMouseToCenter)
	{
		POINT mouseCenter;
		m_CurrentMousePosition.x = GameContext::GetInstance()->pWindow->GetWindowWidth() / 2;
		m_CurrentMousePosition.y = GameContext::GetInstance()->pWindow->GetWindowHeight() / 2;
		mouseCenter.x = m_CurrentMousePosition.x;
		mouseCenter.y = m_CurrentMousePosition.y;
		ClientToScreen(GameContext::GetInstance()->pWindow->GetWindowHandle(), &mouseCenter);

		SetCursorPos(mouseCenter.x, mouseCenter.y);
	}
}

bool RUBENgine::InputManager::IsKeyDown(const int key, const bool isOldState)
{
	if (!m_pKeyBoardStateBuffers[0] || !m_pKeyBoardStateBuffers[1])
		return false;

	//Range Checks
	if (key > 0x00 && key <= 0xFE)
	{
		if (isOldState)
			return (m_pKeyBoardStateBuffers[((int)m_IsKeyBoardBufferZeroActive + 1) % 2][key] & 0xF0) != 0;
		else
			return (m_pKeyBoardStateBuffers[(int)m_IsKeyBoardBufferZeroActive][key] & 0xF0) != 0;
	}

	return false;
}

bool RUBENgine::InputManager::IsKeyPressed(const int key)
{
	return (!IsKeyDown(key, true) && IsKeyDown(key));
}

bool RUBENgine::InputManager::IsKeyReleased(const int key)
{
	return (IsKeyDown(key, true) && !IsKeyDown(key));
}

bool RUBENgine::InputManager::IsKeyState(const int key, const ButtonState state)
{
	return ((int(state) & 1) != IsKeyDown(key, true) && bool(int(state) & 2) != IsKeyDown(key));
}

void RUBENgine::InputManager::UpdateKeyBoardStates()
{
	for (int x = 0; x < 256; x++)
		m_pKeyBoardStateBuffers[((int)m_IsKeyBoardBufferZeroActive + 1) % 2][x] = (char)(GetAsyncKeyState(x) >> 8);
	m_IsKeyBoardBufferZeroActive = !m_IsKeyBoardBufferZeroActive;
}