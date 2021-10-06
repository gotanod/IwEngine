#pragma once

#include "Layers/Menu_Layer.h"

struct Menu_PostGame_Layer : Menu_Layer
{
	UI* m_background;
	UI* m_title;
	UI* m_score_title;
	UI* m_score;
	UI* m_player;
	UI_Button* m_restart;

	Menu_PostGame_Layer()
		: Menu_Layer     ("Menu post game")
					  
		, m_background   (nullptr)
		, m_title        (nullptr)
		, m_score_title  (nullptr)
		, m_score        (nullptr)
		, m_player       (nullptr)
		, m_restart      (nullptr)
	{}

	int Initialize() override;
	void PostUpdate() override;
};
