#pragma once

#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Devices/Mouse.h"

namespace IwInput {
	class IWINPUT_API WindowsMouse
		: public Mouse
	{
	public:
		WindowsMouse(
			InputCallback& callback);

		void HandleEvent(
			OsEvent& event);
	};
}
#endif
