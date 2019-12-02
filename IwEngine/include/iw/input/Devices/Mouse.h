#pragma once

#include "Device.h"

namespace IW {
inline namespace Input {
	class IWINPUT_API MouseBase
		: public Device
	{
	protected:
		static Translation translation;

		static Translation CreateTranslation();

	public:
		MouseBase(
			DeviceType type)
			: Device(type)
		{}

		virtual ~MouseBase() = default;

		static unsigned int Translate(
			InputName key);

		static InputName Translate(
			unsigned int oskey);
	};

	class IWINPUT_API Mouse
		: public MouseBase
	{
	public:
		Mouse()
			: MouseBase(DeviceType::MOUSE)
		{}

		virtual ~Mouse() = default;

		static bool ButtonDown(
			InputName button);

		static bool ButtonUp(
			InputName button);

		static Mouse* Create();
	};

	class IWINPUT_API RawMouse
		: public MouseBase
	{
	public:
		RawMouse()
			: MouseBase(DeviceType::RAW_MOUSE)
		{}

		virtual ~RawMouse() = default;

		static RawMouse* Create();
	};
}
}
