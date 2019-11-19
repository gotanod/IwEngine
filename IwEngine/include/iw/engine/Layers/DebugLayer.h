#pragma once

#include "iw/engine/Layer.h"

namespace IW {
inline namespace Engine {
	class IWENGINE_API DebugLayer
		: public Layer
	{
	private:
		std::vector<std::string> logs;

	public:
		DebugLayer();

		void PostUpdate() override;
		void ImGui() override;

		// Input events

		bool On(IW::MouseWheelEvent& event) override;
		bool On(IW::MouseMovedEvent& event) override;
		bool On(IW::MouseButtonEvent& event) override;
		bool On(IW::KeyEvent& event) override;
		bool On(IW::KeyTypedEvent& event) override;

		// Window events

		bool On(IW::WindowResizedEvent& event) override;
	};
}
}
