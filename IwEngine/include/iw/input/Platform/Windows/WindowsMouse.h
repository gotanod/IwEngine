#pragma once

#ifdef IW_PLATFORM_WINDOWS
#include "iw/input/Devices/Mouse.h"

namespace IW {
inline namespace Input {
	class IWINPUT_API WindowsMouse
		: public Mouse
	{
	public:
		DeviceInput TranslateOsEvent(
			const OsEvent& e) override;
	};
}
}
#endif
