#include "Systems/World_System.h"
#include "iw/engine/Events/Seq/Delay.h"

#include "iw/physics/impl/GJK.h"

//float tt;

void WorldSystem::Update()
{
	//tt += iw::DeltaTime();

	//if (tt > .01)
	//{
	//	tt = 0;

	//	LightningConfig l;
	//	l.X = 100;
	//	l.Y = 200;
	//	l.TargetX = 300;
	//	l.TargetY = 200;
	//	l.LifeTime = .01;
	//	l.ArcSize = 10;
	//	l.Type = LightningType::POINT;

	//	DrawLightning(sand, l);
	//}


	//return;
	m_timer.Tick();

	bool needsAnotherLevel = false;

	if (m_levels.size() > 0)
	{
		if (m_levels.back().Update())
		{
			m_levels.pop_back();
			needsAnotherLevel = true;
		}
	}

	else {
		needsAnotherLevel = true;
	}

	if (needsAnotherLevel)
	{
		int enemyCount = 0;
		Space->Query<Enemy>().Each([&](iw::EntityHandle handle, Enemy*) {
			enemyCount++;
		});

		iw::EventSequence& seq = m_levels.emplace_front(CreateSequence());
		if (enemyCount < 15)
		{
			seq.Add<Spawn>(MakeEnemySpawner())
			   .And<Spawn>(MakeAsteroidSpawner())
			   .And<iw::Delay>(10);
		}

		else {
			seq.Add<Spawn>(MakeAsteroidSpawner());
		}

		seq.Start();
	}

	Space->Query<iw::Transform, iw::Tile, Asteroid>().Each(
	[&](
		iw::EntityHandle handle, 
		iw::Transform* transform,
		iw::Tile* tile,
		Asteroid* asteroid) 
	{
		asteroid->Lifetime -= iw::DeltaTime();

		iw::AABB2 b = iw::TransformBounds(tile->m_bounds, transform);

		if (   asteroid->Lifetime < 0.f
			&& !iw::AABB2(glm::vec2(200.f), 200).Intersects(&iw::Transform(), tile->m_bounds, transform))
		{
			Space->QueueEntity(handle, iw::func_Destroy);
		}
	});
}

void WorldSystem::FixedUpdate()
{
	Space->Query<Throwable, Flocker>().Each(
		[&](
			iw::EntityHandle handle, 
			Throwable* throwable, 
			Flocker* flocker) 
		{
			if (throwable->Held)
			{
				flocker->Active = false;
			}

			if (   !flocker->Active 
				&& !throwable->Held)
			{
				throwable->Timer += iw::FixedTime();

				if (throwable->Timer > throwable->Time + .5)
				{
					flocker->Active = true;
				}
			}
		});

	Space->Query<iw::Rigidbody, Throwable>().Each(
		[&](
			iw::EntityHandle handle, 
			iw::Rigidbody* rigidbody,
			Throwable* throwable) 
		{
			if (throwable->Held)
			{
				if (!throwable->ThrowRequestor.Alive())
				{
					return;
				}

				throwable->Timer += iw::FixedTime();

				float distFromThrower = glm::distance(
					rigidbody->Transform.Position, 
					throwable->ThrowRequestor.Find<iw::Transform>()->Position);

				if (   throwable->Timer > throwable->Time
					|| distFromThrower  > 150)
				{
					throwable->Held = false;
				}

				glm::vec3 pos  = rigidbody->Transform.Position;
				glm::vec3 tpos = throwable->ThrowTarget   .Find<iw::Transform>()->Position;
				glm::vec3 rpos = throwable->ThrowRequestor.Find<iw::Transform>()->Position;

				/*
				auto [min, max] = rigidbody->Collider->as<iw::Physics::impl::Collider<iw::d2>>()->Bounds();
				float radius = glm::length(max - min) / 2.f;
				*/

				glm::vec3 vel = tpos - pos + (pos - rpos) / 2.f;
				vel = glm::normalize(vel);

				float currentSpeed = glm::length(rigidbody->Velocity);
				float speed = iw::max(currentSpeed, 200.f * throwable->Timer / throwable->Time);

				rigidbody->Velocity = iw::lerp(rigidbody->Velocity, vel * speed, iw::FixedTime() * 25);

				LightningConfig config;
				config.A = throwable->ThrowRequestor;
				config.B = Space->GetEntity(handle);
				config.ArcSize = 10;
				config.LifeTime = .01f;

				DrawLightning(sand, Space, config);
			}
		});
}

bool WorldSystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case PROJ_HIT_TILE:
		{
			auto& [x, y, info, hit, projectile] = e.as<ProjHitTile_Event>().Config;
			
			if (   hit.Has<Player>() 
				|| hit.Has<Enemy>())
			{
				glm::vec3 norm;

				if (iw::Rigidbody* pbody = projectile.Find<iw::Rigidbody>())
				{
					norm = glm::normalize(pbody->Velocity);
				}

				else {
					norm = glm::normalize(glm::vec3(iw::randfs(), iw::randfs(), 0.f));
				}

				iw::Cell spark;
				spark.Type  = iw::CellType::ROCK;
				spark.Props = iw::CellProp::MOVE_FORCE;
				
				spark.Color = iw::Color::From255(255, 207, 77);
				
				spark.dx = norm.x * 300 + iw::randfs() * 100;
				spark.dy = norm.y * 300 + iw::randfs() * 100;
				spark.life = .05 + iw::randf() * .2;

				float margin = 15;
				float px = x + norm.x * margin;
				float py = y + norm.y * margin;
				
				sand->m_world->SetCell(px, py, spark);
			}

			// todo: if entity is an asteroid
			// eject dirt or something, gravitized to nearest asteroid

			break;
		}
		case REMOVE_CELL_FROM_TILE:
		{
			RemoveCellFromTile_Event& event = e.as<RemoveCellFromTile_Event>();
			sand->EjectPixel(event.Entity.Find<iw::Tile>(), event.Index);

			break;
		}
		case CORE_EXPLODED:
		{
			iw::Entity& entity = e.as<CoreExploded_Event>().Entity;

			if (!entity) return false;

			glm::vec3 pos = entity.Find<iw::Transform>()->Position;

			SpawnExplosion_Config config;
			config.SpawnLocationX = pos.x;
			config.SpawnLocationY = pos.y;
			config.ExplosionRadius = 1;

			if (entity.Has<Enemy>())
			{
				config.ExplosionPower = entity.Find<Enemy>()->ExplosionPower;
			}

			else 
			{
				CorePixels* core = entity.Find<CorePixels>();
				for (int i : core->ActiveIndices)
				{
					Bus->push<RemoveCellFromTile_Event>(i, entity);
				}

				config.ExplosionPower = 50; // player explosion
			}

			Bus->push<SpawnExplosion_Event>(config);

			break;
		}
		case SPAWN_ASTEROID: {
			MakeAsteroid(e.as<SpawnAsteroid_Event>().Config);
			break;
		}
		case CREATED_PLAYER: {
			m_player = e.as<CreatedPlayer_Event>().PlayerEntity;

			break;
		}
		case STATE_CHANGE:
		{
			StateChange_Event& event = e.as<StateChange_Event>();

			switch (event.State)
			{
				case GAME_START_STATE:
				{
					//SpawnAsteroid_Config c;
					//c.SpawnLocationX = 100;
					//c.SpawnLocationY = 100;
					//c.Size = 0;

					//Bus->push<SpawnAsteroid_Event>(c);

					//SpawnEnemy_Config cc;
					//cc.SpawnLocationX = 300;
					//cc.SpawnLocationY = 300;
					//cc.TargetLocationX = 300;
					//cc.TargetLocationY = 300;
					//cc.EnemyType = STATION;

					//Bus->push<SpawnEnemy_Event>(cc);

					m_levels.emplace_front(CreateSequence())
						.Add<Spawn>(MakeEnemySpawner())
						.And<Spawn>(MakeAsteroidSpawner())
						.And<iw::Delay>(30)
						.Start();
					break;
				}
				case GAME_OVER_STATE:
				{
					m_levels.clear();
					break;
				}
			}

			break;
		}
	}

	return false;
}

iw::Entity WorldSystem::MakeAsteroid(
	SpawnAsteroid_Config& config)
{
	iw::ref<iw::Texture> asteroid_tex;

	switch (config.Size)
	{
		case 0: asteroid_tex = A_texture_asteroid_mid_1; break;
		case 1: asteroid_tex = A_texture_asteroid_mid_2; break;
		case 2: asteroid_tex = A_texture_asteroid_mid_3; break;
	}

	iw::Entity entity = sand->MakeTile<iw::MeshCollider2, Asteroid, Throwable>(*asteroid_tex, true);
	iw::Transform* t = entity.Find<iw::Transform>();
	iw::Rigidbody* r = entity.Find<iw::Rigidbody>();

	t->Position = glm::vec3(config.SpawnLocationX, config.SpawnLocationY, 0);

	r->Velocity = glm::vec3(config.VelocityX, config.VelocityY, 0);
	r->AngularVelocity.z = config.AngularVel;
	r->SetTransform(t);
	r->SetMass(1000);

	entity.Find<Asteroid>()->Lifetime = 10;

	return entity;
}

//void WorldSystem::SetupLevels()
//{
//	m_runningLevels.clear();
//	m_levels.clear();
//
//	// should load from files
//
//	auto [w, h] = sand->GetSandTexSize();
//	sand->SetCamera(w / 2, h / 2);
//
//	iw::EventSequence& level1 = m_levels.emplace_back(CreateSequence());
//
//	Spawn spawner(10, 2, .5, .3);
//	spawner.AddSpawn(w / 2 - 50, h + 10, w / 2 + 50, h + 100);
//	spawner.OnSpawn = SpawnEnemy;
//
//	level1.Add([&]()
//		{
//			SpawnAsteroid_Config config;
//			SpawnAsteroid_Config config1;
//		
//			config.SpawnLocationX = 0;
//			config.SpawnLocationY = 550;
//			config.VelocityX =  20;
//			config.VelocityY = -100;
//			config1.AngularVel = -iw::randfs() / 3;
//
//			config1.SpawnLocationX = 400;
//			config1.SpawnLocationY = 550;
//			config1.VelocityX = -20;
//			config1.VelocityY = -100;
//			config1.AngularVel = iw::randfs() / 3;
//
//			Bus->push<SpawnAsteroid_Event>(config);
//			Bus->push<SpawnAsteroid_Event>(config1);
//
//			return true;
//		})
//		->Add<iw::Delay>(15)
//		->Add<Spawn>(spawner)
//		->Add([&]() {
//			m_levels.pop_back();
//			return true;
//		});
//
//	m_levels.back().Start();
//}

//Spawn WorldSystem::MakeSpawner(
//	int numb, int batch,
//	float time, float timeMargin,
//	std::initializer_list<WhereToSpawn> where)
//{
//	Spawn spawn(numb, batch, time, timeMargin);
//	
//	auto [w, h] = sand->GetSandTexSize();
//
//	for (WhereToSpawn side : where)
//	{
//		switch (side)
//		{
//			case TOP:    spawn.AddSpawn(  - 100, h + 100, w + 100, h + 200); break;
//			case BOTTOM: spawn.AddSpawn(  - 100,   - 100, w + 100,   - 200); break;
//			case LEFT:   spawn.AddSpawn(w - 100,   - 100, w - 200, h + 200); break;
//			case RIGHT:  spawn.AddSpawn(w + 100,   - 100, w + 200, h + 200); break;
//		}
//	}
//
//	return spawn;
//}
//
//Spawn WorldSystem::MakeEnemySpawnner(
//	int numb, int batch,
//	float time, float timeMargin)
//{
//	Spawn spawn = MakeSpawner(numb, batch, time, timeMargin, where);
//	spawn.func_Spawn = [=](float x, float y)
//	{
//		auto [w, h] = sand->GetSandTexSize();
//		float margin = .1f;
//
//		float target_x = iw::randi(w - w * margin * 2) + w * margin;
//		float target_y = iw::randi(h - h * margin * 2) + h * margin;
//
//		Bus->push<SpawnEnemy_Event>(m_player, x, y, target_x, target_y);
//	};
//	
//	return spawn;
//}
//
//Spawn WorldSystem::MakeAsteroidSpawnner(
//	std::initializer_list<WhereToSpawn> where)
//{
//	Spawn spawn = MakeSpawner(where, 3, 1, 2, 1);
//	spawn.func_Spawn = [=](float x, float y)
//	{
//		SpawnAsteroid_Config config;
//		config.SpawnLocationX = x;
//		config.SpawnLocationY = y;
//		config.VelocityX = iw::randfs() * 10;
//		config.VelocityY = -20 - iw::randfs() * 10;
//		config.AngularVel = iw::randfs() / 10;
//		config.Size = iw::randi(10);
//
//		Bus->push<SpawnAsteroid_Event>(config);
//	};
//
//	return spawn;
//}

//Spawn WorldSystem::MakeAsteroidSpawnner()
//{
//	return Spawn();
//}
