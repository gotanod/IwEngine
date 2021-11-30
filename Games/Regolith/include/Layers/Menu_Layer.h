#pragma once

#include "iw/engine/Layer.h"
#include "iw/engine/UI.h"
#include "Assets.h"

struct UI_Layer : iw::Layer
{
	UI_Screen* m_screen;

	UI_Layer(
		const std::string& name
	)
		: iw::Layer (name)
		, m_screen  (nullptr)
	{}

	virtual void Destroy() override
	{
		Space->FindEntity(m_screen).Destroy();
		m_screen = nullptr;

		Layer::Destroy();
	}

	virtual void OnPush() override
	{
		if (m_screen)
		{
			Space->FindEntity(m_screen).Revive();
		}

		else {
			iw::Mesh default = iw::ScreenQuad().MakeInstance();
			default.Material = A_material_texture_cam->MakeInstance();
			m_screen = Space->CreateEntity<UI_Screen>().Set<UI_Screen>(default);
		}

		Layer::OnPush();
	}

	virtual void OnPop() override
	{
		Space->FindEntity(m_screen).Kill();

		Layer::OnPop();
	}
};

struct Menu_Layer : UI_Layer
{
	iw::HandlerFunc* m_handle;
	bool m_execute;
	bool m_last_execute;

	Menu_Layer(
		const std::string& name
	)
		: UI_Layer       (name)
					  
		, m_handle       (nullptr)
		, m_execute      (false)
		, m_last_execute (false)
	{}

	void ButtonUpdate()
	{
		// find all buttons

		std::vector<UI_Base*> clickables;

		m_screen->WalkTree([&](UI_Base* ui, UI_Base* parent)
		{
			if (dynamic_cast<UI_Clickable*>(ui))
			{
				clickables.push_back(ui);
			}
		});

		for (UI_Base* ui : clickables)
		{
			UI_Clickable* clickable = dynamic_cast<UI_Clickable*>(ui);

			if (ui->IsPointOver(m_screen->LocalMouse()))
			{
				if (clickable->whileMouseHover)
				{
					clickable->whileMouseHover();
				}

				if (    m_execute
					&& clickable->whileMouseDown)
				{
					clickable->whileMouseDown();
				}

				else
				if (    m_last_execute
					&& clickable->onClick)
				{
					clickable->onClick();
				}
			}

			ui->x = 0;
			ui->y = 0;
		}

		m_last_execute = m_execute;
	}
	
	virtual void OnPush() override
	{
		m_handle = Console->AddHandler([&](
			const iw::Command& command) 
		{
			if (command.Verb == "execute")
			{
				m_execute = command.Active;
			}

			return false;
		});

		UI_Layer::OnPush();
	}

	virtual void OnPop() override
	{
		Console->RemoveHandler(m_handle);

		m_handle = nullptr;
		m_execute = false;
		m_last_execute = false;

		UI_Layer::OnPop();
	}
};
