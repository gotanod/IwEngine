#pragma once

#include "iw/engine/Events/ActionEvents.h"
#include "iw/entity/Entity.h"
#include "iw/graphics/Color.h"

enum Actions {
	SPAWN_ENEMY,
	SPAWN_PROJECTILE,
};

struct SpawnEnemy_Event : iw::SingleEvent
{
	iw::Entity ShootAt;
	float TargetLocationX;
	float TargetLocationY;

	SpawnEnemy_Event(
		iw::Entity shootAt,
		float targetLocationX,
		float targetLocationY
	)
		: iw::SingleEvent(SPAWN_ENEMY)
		, ShootAt(shootAt)
		, TargetLocationX(targetLocationX)
		, TargetLocationY(targetLocationY)
	{}
};

struct SpawnProjectile_Event : iw::SingleEvent
{
	float X, Y, dX, dY;

	enum PType {
		BULLET,
		LASER
	} Type;

	SpawnProjectile_Event(
		float x,  float y,
		float dx, float dy,
		PType type
	)
		: iw::SingleEvent(SPAWN_PROJECTILE)
		, X(x)
		, Y(y)
		, dX(dx)
		, dY(dy)
		, Type(type)
	{}
};