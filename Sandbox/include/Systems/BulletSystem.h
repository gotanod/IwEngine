#pragma once

#include "Components/Bullet.h"
#include "iw/engine/System.h"
#include "iw/data/Components/Transform.h"

class BulletSystem
	: public IwEngine::System<IwEngine::Transform, Bullet>
{
public:
	BulletSystem(
		IwEntity::Space& space,
		IwGraphics::RenderQueue& renderQueue);

	~BulletSystem();

	void Update(
		View& view) override;
};