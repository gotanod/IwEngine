#pragma once

#include "iw/engine/Events/ActionEvents.h"

enum class Actions
	: int
{
	JUMP, RIGHT, FORWARD, USE,
	SPAWN_CIRCLE_TEMP,
	RESET_LEVEL, NEXT_LEVEL, OPEN_NEXT_LEVEL, LOADED_LEVEL
};

struct JumpEvent
	: iw::ToggleEvent
{
	JumpEvent(
		bool active)
		: ToggleEvent(iw::val(Actions::JUMP), active)
	{}
};

struct RightEvent
	: iw::ToggleEvent
{
	RightEvent(
		bool active)
		: iw::ToggleEvent(iw::val(Actions::RIGHT), active)
	{}
};

struct ForwardEvent
	: iw::ToggleEvent
{
	ForwardEvent(
		bool active)
		: iw::ToggleEvent(iw::val(Actions::FORWARD), active)
	{}
};

struct UseEvent
	: iw::SingleEvent
{
	UseEvent()
		: iw::SingleEvent(iw::val(Actions::USE))
	{}
};

struct ResetLevelEvent
	: iw::SingleEvent
{
	ResetLevelEvent()
		: iw::SingleEvent(iw::val(Actions::RESET_LEVEL))
	{}
};

struct NextLevelEvent
	: iw::SingleEvent
{
	NextLevelEvent()
		: iw::SingleEvent(iw::val(Actions::NEXT_LEVEL))
	{}
};

struct LoadedLevelEvent
	: iw::SingleEvent
{
	LoadedLevelEvent()
		: iw::SingleEvent(iw::val(Actions::LOADED_LEVEL))
	{}
};

struct OpenNextLevelEvent
	: iw::SingleEvent
{
	OpenNextLevelEvent()
		: iw::SingleEvent(iw::val(Actions::OPEN_NEXT_LEVEL))
	{}
};
