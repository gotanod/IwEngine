#include "iw/input/InputManager.h"
#include"iw/input/Devices/Mouse.h"

namespace IwInput {
	void InputManager::HandleEvent(
		int wid,
		int msg,
		short wParam,
		int lParam)
	{
		m_deviceManager.HandleEvent({ wid, msg, wParam, lParam });
	}

	void InputManager::CreateContext(
		unsigned int windowId)
	{
		CreateContext(windowId, NO_WIDTH, NO_HEIGHT);
	}

	void InputManager::CreateContext(
		unsigned int windowId,
		float width,
		float height)
	{
		m_contextManager.CreateContext(windowId, width, height);
	}

	template<>
	void InputManager::CreateDevice<Mouse>() {
		m_deviceManager.CreateDevice<Mouse>(iwevents::make_callback(
			&ContextManager::HandleInput, &m_contextManager));
	}

	template<>
	void InputManager::CreateDevice<Keyboard>() {
		m_deviceManager.CreateDevice<Keyboard>(iwevents::make_callback(
			&ContextManager::HandleInput, &m_contextManager));
	}

#ifdef IW_PLATFORM_WINDOWS
	template<>
	void InputManager::CreateDevice<RawMouse>() {
		m_deviceManager.CreateDevice<RawMouse>(iwevents::make_callback(
			&ContextManager::HandleInput, &m_contextManager));
	}
#endif

	void InputManager::SetMouseWheelCallback(
		unsigned int windowId,
		MouseWheelCallback callback)
	{
		m_contextManager.SetMouseWheelCallback(windowId, callback);
	}

	void InputManager::SetMouseMovedCallback(
		unsigned int windowId,
		MouseMovedCallback callback)
	{
		m_contextManager.SetMouseMovedCallback(windowId, callback);
	}

	void InputManager::SetMouseButtonCallback(
		unsigned int windowId,
		MouseButtonCallback callback)
	{
		m_contextManager.SetMouseButtonCallback(windowId, callback);
	}

	void InputManager::SetKeyCallback(
		unsigned int windowId,
		KeyCallback callback)
	{
		m_contextManager.SetKeyCallback(windowId, callback);
	}
}