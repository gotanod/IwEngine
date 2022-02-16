#pragma once

#include "Layers/Menu_Layer.h"

#include "gl/glew.h"
#include "gl/wglew.h"

#include <functional>

#include "MenuParts.h"
#include "Settings.h"

enum class MenuTarget
{
	DEFAULT,
	HIGHSCORES,
	SETTINGS,
	SETTINGS_CONTROLS,
	PAUSE,
	GAME,
	POST_GAME // input name for highscore - and replay of death
};

struct Menu_Title_Layer : Menu_Layer
{
	iw::Entity axis;

	iw::Entity stars;

	bool drawMenubg;
	iw::Entity ball;
	iw::Entity title;
	iw::Entity title_hs;
	iw::Entity title_st;
	iw::Entity title_ps;
	iw::Entity smoke;

	iw::ref<iw::RenderTarget> bg;
	iw::PerspectiveCamera persp;
	iw::OrthographicCamera ortho;
	glm::vec3 target_pos;
	glm::quat target_rot;
	glm::vec3 last_pos;
	glm::quat last_rot;

	float t;
	float t1;

	float pers;
	float target_pers;
	float last_pers;

	MenuTarget target_menu;
	MenuTarget last_menu;

	float fade;
	float target_fade;

	float fade_exit;
	float __fps;

	bool normalize;

	Highscores_MenuParts highscoreParts;

	GameSettings gameSettings; // for audio/video
	GameSettings gameControls; // for button mappings

	std::stack<std::pair<MenuTarget, std::function<void()>>> BackState; // GoBack pops these

	Menu_Title_Layer()
		: Menu_Layer  ("Menu Title")
		, t           (1.f)
		, t1          (1.f)
		, pers        (0.f)
		, fade        (0.f)
		, target_fade (0.f)
		, fade_exit   (0.f)
		, normalize   (true)
	{
		SetViewDefault();
		last_menu = target_menu;
		last_pers = target_pers;
		last_pos = target_pos;
		last_rot = target_rot;
	}

	void SetViewDefault()
	{
		target_menu = MenuTarget::DEFAULT;
		target_pers = 0.f;
		target_pos = glm::vec3(0, 0, 10);
		target_rot = glm::quat(1, 0, 0, 0);
		target_fade = 0.f;
		drawMenubg = true;
		normalize = true;
	}

	void SetViewHighscores()
	{
		target_menu = MenuTarget::HIGHSCORES;
		target_pers = 0.f;
		target_pos = glm::vec3(10, 0, 0);
		target_rot = glm::quat(sqrt(2) / 2, 0, sqrt(2) / 2, 0);
		target_fade = 0.f;
		drawMenubg = true;
		normalize = true;
	}

	void SetViewSettings()
	{
		target_menu = MenuTarget::SETTINGS;
		target_pers = 0.f;
		target_pos = glm::vec3(7.4, -8, 5);
		target_rot = glm::quat(.98, 0, .2, 0);
		target_fade = 0.f;
		drawMenubg = true;
		normalize = true;
	}

	void SetViewPause()
	{
		target_menu = MenuTarget::PAUSE;
		//target_pers = 1.f;
		target_fade = 0.6f;
		normalize = true;
	}

	void SetViewGame()
	{
		target_menu = MenuTarget::GAME; // no menu
		target_pers = 1.f;
		target_pos = glm::vec3(100, 100, 100);
		target_rot = glm::quat(1, 0, 0, 0);
		target_fade = 0.f;
		normalize = false;

		Task->delay(1.f, [&]()
		{
			drawMenubg = false;
		});
	}

	bool IsFromPause() const
	{
		return BackState.size() == 0
			|| BackState.top().first == MenuTarget::PAUSE;
	}

	void PushBackState(MenuTarget target, std::function<void()> func = {})
	{
		BackState.emplace(target, func);
	}

	void GoBack()
	{
		if (BackState.size() == 0)
		{
			return;
		}

		auto [target, func] = BackState.top(); BackState.pop();

		switch (target)
		{
			case MenuTarget::DEFAULT:     SetViewDefault();    break;
			case MenuTarget::HIGHSCORES:  SetViewHighscores(); break;
			case MenuTarget::SETTINGS:    SetViewSettings();   break;
			case MenuTarget::PAUSE:       SetViewPause();      break;
			case MenuTarget::GAME:        SetViewGame();       break;
		}

		if (func)
		{
			func();
		}
	}

	void Escape()
	{
		if (    target_menu != MenuTarget::GAME /*
			&& BackButtonTarget == MenuTarget::DEFAULT*/)
		{
			Bus->push<PlaySound_Event>("event:/ui/click");
			GoBack();
		}
	}

	int Initialize() override;
	void UI() override;
};
//https://github.com/blender/blender/blob/master/source/blender/gpu/shaders/material/gpu_shader_material_tex_noise.glsl
