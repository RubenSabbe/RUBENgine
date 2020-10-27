#include "InputManager.h"

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

		//First check/previous frame: will return false if currAction->TriggerState == Pressed||1
		//Second check/current frame: will return false if currAction->TriggerState == Released||0
		if (bool(int(currAction->TriggerState) % 2) != IsKeyDown(currAction->KeyCode, true) && bool(currAction->TriggerState) == IsKeyDown(currAction->KeyCode))
			currAction->IsTriggered = true;
	}

	//TODO: Replace with actual values once window exists
	int GETWINDOWWIDTH = 0;
	int GETWINDOWHEIGHT = 0;
	HWND GETWINDOWHANDLE = 0;

	//Mouse Position
	m_OldMousePosition = m_CurrentMousePosition;
	if (GetCursorPos(&m_CurrentMousePosition))
	{
		ScreenToClient(GETWINDOWHANDLE, &m_CurrentMousePosition);
	}

	m_MouseMovement.x = m_CurrentMousePosition.x - m_OldMousePosition.x;
	m_MouseMovement.y = m_CurrentMousePosition.y - m_OldMousePosition.y;

	if (m_ForceToCenter)
	{
		POINT mouseCenter;
		m_CurrentMousePosition.x = GETWINDOWWIDTH / 2;
		m_CurrentMousePosition.y = GETWINDOWHEIGHT / 2;
		mouseCenter.x = m_CurrentMousePosition.x;
		mouseCenter.y = m_CurrentMousePosition.y;
		ClientToScreen(GETWINDOWHANDLE, &mouseCenter);

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
	//TODO: Replace with actual values once window exists
	int GETWINDOWWIDTH = 0;
	int GETWINDOWHEIGHT = 0;
	HWND GETWINDOWHANDLE = 0;

	m_ForceToCenter = forceMouseToCenter;

	if (forceMouseToCenter)
	{
		POINT mouseCenter;
		m_CurrentMousePosition.x = GETWINDOWWIDTH / 2;
		m_CurrentMousePosition.y = GETWINDOWHEIGHT / 2;
		mouseCenter.x = m_CurrentMousePosition.x;
		mouseCenter.y = m_CurrentMousePosition.y;
		ClientToScreen(GETWINDOWHANDLE, &mouseCenter);

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

bool RUBENgine::InputManager::UpdateKeyBoardStates()
{
	BOOL getKeyboardResult;
	getKeyboardResult = GetKeyboardState(m_pKeyBoardStateBuffers[((int)m_IsKeyBoardBufferZeroActive + 1) % 2]);
	m_IsKeyBoardBufferZeroActive = !m_IsKeyBoardBufferZeroActive;
	return getKeyboardResult > 0;
}