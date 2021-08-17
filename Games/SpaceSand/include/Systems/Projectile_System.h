#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "Components/Projectile.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "Events.h"

struct ProjectileSystem
	: iw::SystemBase
{
	iw::SandLayer* sand;

	std::vector<std::tuple<int, int, float>> m_cells; // x, y, life

	ProjectileSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("Projectile")
		, sand(sand)
	{}

	int Initialize() override;
	void Update() override;
	void FixedUpdate() override;

	bool On(iw::ActionEvent& e) override;

private:
	iw::Entity MakeProjectile(float x, float y, float dx, float dy);

	iw::Entity MakeBullet(float x, float y, float dx, float dy, int depth);
	iw::Entity MakeLaser (float x, float y, float dx, float dy, int depth);

	void MakeExplosion(int x, int y, int r);
};
