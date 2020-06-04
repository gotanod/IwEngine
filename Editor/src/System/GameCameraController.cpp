#include "Systems/GameCameraController.h"
#include "Events/ActionEvents.h"
#include "iw/engine/Time.h"
#include <imgui/imgui.h>

GameCameraController::GameCameraController(
	iw::Entity& target,
	iw::Scene* scene)
	: System("Game Camera Controller")
	, m_target(target)
	, scene(scene)
	, locked(false)
	, follow(true)
	, transitionToCenter(false)
	, speed(2.0)
{}

float y = 27.15f;

void GameCameraController::Update(
	iw::EntityComponentArray& eca)
{
	if (locked == true) {
		return;
	}

	for (auto entity : eca) {
		auto [t, c] = entity.Components.Tie<Components>();

		iw::vector3 target;
		if (follow) {
			target = m_target.Find<iw::Transform>()->Position;
		}

		else {
			target = center;
		}

		target.y = y;

		iw::quaternion camrot =
			  iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2)
			* iw::quaternion::from_axis_angle(iw::vector3::unit_z, iw::Pi);

		t->Position = iw::lerp(t->Position, target, iw::Time::DeltaTime() * speed);
		t->Rotation = iw::lerp(t->Rotation, camrot, iw::Time::DeltaTime() * speed);

		//iw::PointLight* lamp = scene->PointLights()[0];

		//iw::vector3 subpos = m_target.Find<iw::Transform>()->Position;
		//subpos.y = 1;

		//lamp->SetPosition(subpos);
	}
}

bool GameCameraController::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::GOTO_NEXT_LEVEL): {
			follow = false;

			GoToNextLevelEvent& event = e.as<GoToNextLevelEvent>();

			//center.x = event.CenterPosition.x;
			//center.z = event.CenterPosition.y;

			if (event.LevelName == "levels/forest/forest23.json") {
				y = 41.25f;
			}

			else if (event.LevelName == "levels/forest/forest05.a.json") {
				y = 17.15f;
			}

			else {
				y = 27.15f;
			}

			//if (event.CameraFollow) {
			//	center.x += event.PlayerPosition.x;
			//	center.z += event.PlayerPosition.y;
			//}

			break;
		}
		case iw::val(Actions::START_LEVEL): {
			StartLevelEvent& event = e.as<StartLevelEvent>();

			follow = event.CameraFollow;

			if (event.LevelName == "levels/forest/forest23.json") { // dont need only for starting in debug mode
				y = 41.25f;
			}

			else if (event.LevelName == "levels/forest/forest05.a.json") {
				y = 17.15f;
			}

			else {
				y = 27.15f;
			}

			break;
		}
		case iw::val(Actions::DEV_CONSOLE): {
			locked = !e.as<iw::ToggleEvent>().Active;
		}
	}

	return false;
}
