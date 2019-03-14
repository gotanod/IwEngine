#pragma once

#include "iw/engine/Events/AllEvents.h"
#include "iw/log/logger.h"
#include "iw/engine/Platform.h"

namespace IwEngine {
	template<
		typename _event_t>
	_event_t Translate(
		int event,
		int wParam,
		long lParam)
	{
		LOW_WARNING << "No translation exists for event " << event
			<< " {wParam = " << wParam << ", lParam" << lParam << "}";

		return _event_t();
	}

	template<>
	WindowResizedEvent Translate(
		int event,
		int wParam,
		long lParam)
	{
		return WindowResizedEvent(LOWORD(lParam), HIWORD(lParam));
	}

	template<>
	MouseButtonPressedEvent Translate(
		int event,
		int wParam,
		long lParam)
	{
		if (event == WM_LBUTTONDOWN) {
			return MouseButtonPressedEvent(GET_X_LPARAM())
		}
		return MouseButtonPressedEvent(LOWORD(lParam), HIWORD(lParam));
	}
}