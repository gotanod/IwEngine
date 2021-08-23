#pragma once

#include "iw/engine/Components/Timer.h"
#include "Components/Projectile.h"
#include "Components/Weapon.h"

struct Player
{
	bool i_up; // inputs, should use event loop
	bool i_down;
	bool i_left;
	bool i_right;
	bool i_fire1;
	bool i_fire2;

	bool can_fire_laser;

	Player() {}
};
