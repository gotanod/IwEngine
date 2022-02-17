#pragma once 

// for event recording
#include "Events.h"
//#include "iw/util/reflection/serialization/Serializer.h"
//#include "iw/reflected/reflected.h"

#include "iw/engine/EntryPoint.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/math/noise.h"

#include "iw/engine/Layers/DebugLayer.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"

#include "Layers/Game_Layer.h"
#include "Layers/Game_UI_Layer.h"
#include "Layers/Menu_Title_Layer.h"
#include "Layers/Menu_Fadeout_Layer.h"
#include "Layers/Menu_Upgrade_Layer.h"
#include "Layers/Audio_Layer.h"
#include "Layers/Menu_Bg_Render_Layer.h"

#include "State.h"
#include "MenuState.h"
#include <stack>

#include "iw/util/io/File.h"

struct StaticLayer : iw::Layer
{
	StaticLayer()
		: iw::Layer("Static")
	{}

	int Initialize() override
	{
		PushSystem<iw::PhysicsSystem>();
		PushSystem<iw::EntityCleanupSystem>();

		if (int e = LoadAssets(Asset.get(), Renderer->Now.get()))
		{
			return e;
		}

		Physics->AddSolver(new iw::SmoothPositionSolver());
		Physics->AddSolver(new iw::ImpulseSolver());

		return Layer::Initialize();
	}
};

class App
	: public iw::Application
{
private:
	iw::FontMap* m_fonts;

	Menu_Title_Layer*  m_menus;
	Game_Layer*        m_game;
	Menu_GameUI_Layer* m_gameUI;

	StateName m_state;

	void ChangeState(StateName state)
	{
		m_state = state;
		Bus->push<StateChange_Event>(state);
	}

public:
	App()
		: iw::Application ()
		, m_fonts  (nullptr)
		, m_menus  (nullptr)
		, m_game   (nullptr)
		, m_gameUI (nullptr)
		, m_state  (StateName::IN_MENU)
	{}

	void ChangeToPauseMenu()
	{
		m_menus->PushBackState(MenuTarget::GAME, [this]() { ChangeBackToGame(); });
		m_menus->SetViewPause();
		PopLayer(m_game->sand);
		PopLayer(m_game->sand_ui_laserCharge);
		PopLayer(m_game);
		//PopLayer(m_gameUI);
		Input->SetContext("menu");
		Physics->Paused = true;
		ChangeState(StateName::IN_GAME_THEN_MENU);
	}

	void ChangeBackToGame()
	{
		m_menus->PushBackState(MenuTarget::DEFAULT);
		m_menus->SetViewGame(); // some random place with stars
		PushLayerFront(m_game->sand);
		PushLayerFront(m_game->sand_ui_laserCharge);
		PushLayerFront(m_game);
		Input->SetContext("game");
		Physics->Paused = false;
		ChangeState(StateName::IN_GAME);
	}

	void ChangeToTitleScreen()
	{
		m_menus = PushLayer<Menu_Title_Layer>();
		PushLayer<Menu_Bg_Render_Layer>();
		Input->SetContext("menu");

		m_state = StateName::IN_MENU;
	}

	void ChangeToGame()
	{
		iw::SandLayer* sand          = new iw::SandLayer(2, 1, 800, 800, 4, 4, false);
		iw::SandLayer* sand_ui_laser = new iw::SandLayer(1, 1, 40,  40);
		sand         ->m_updateDelay = 1 / 144.f;
		sand_ui_laser->m_updateDelay = 1 / 60.f;
	
		m_game   = new Game_Layer       (sand, sand_ui_laser);
		m_gameUI = new Menu_GameUI_Layer(sand, sand_ui_laser, m_menus->bg);

		PushLayer(sand);
		PushLayer(sand_ui_laser);
		PushLayer(m_game);
		PushLayer(m_gameUI);

		Input->SetContext("game");

		m_state = StateName::IN_GAME;
	}

	void ChangeToPost()
	{
		DestroyLayer(m_game->sand);
		DestroyLayer(m_game->sand_ui_laserCharge);
		DestroyLayer(m_gameUI);
		DestroyLayer(m_game);

		//PushLayer(m_post);

		Input->SetContext("menu");
		m_menus->SetViewHighscores(/* here should be a flag for if it's post game or from main menu */);

		m_state = StateName::IN_MENU;
	}

	void SetState(
		const std::string& stateName)
	{
		     if (stateName == "menus") ChangeToTitleScreen();
		else if (stateName == "game")  ChangeToGame();
		else if (stateName == "post")  ChangeToPost();
		else
		{
			LOG_ERROR << "[set-state] invalid state";
		}
	}

	int Initialize(iw::InitOptions& options) override;
};
