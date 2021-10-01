#pragma once

#include "iw/engine/Layer.h"
#include <vector>
#include <string>

enum StateName {
	NONE,

	//GAME_PLAYING,
	//GAME_POST,
	//MENU_MAIN,
	//MENU_TITLE,
	//MENU_PAUSE,
	//MENU_BUTTONS,
	//MENU_SETTINGS_VIDEO,
	//MENU_SETTINGS_AUDIO,

/*
	NONE,
*/
	GAME_OVER_STATE,
	GAME_START_STATE,
	GAME_PAUSE_STATE,
	GAME_RESUME_STATE,
};

struct GameState {
	std::string Name;
	std::vector<iw::Layer*> Layers;
	StateName State;
	std::function<void()> OnChange;

	GameState(
		std::string name,
		StateName state = NONE
	)
		: Name(name)
		, State(state)
	{}
};