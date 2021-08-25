#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "iw/physics/Collision/SphereCollider.h"
#include "Components/Player.h"
#include "Components/KeepInWorld.h"
#include "Components/Item.h"

#include "Events.h"
#include "Assets.h"

struct PlayerSystem : iw::SystemBase
{
	iw::SandLayer* sand;
	iw::Entity m_player;

	float cam_x, cam_y;

	PlayerSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("Player")
		, sand(sand)
		, cam_x(0)
		, cam_y(0)
	{}

	int Initialize() override;
	void FixedUpdate() override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;
};
