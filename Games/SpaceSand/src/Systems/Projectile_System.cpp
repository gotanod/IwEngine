#include "Systems/Projectile_System.h"

int ProjectileSystem::Initialize()
{
	return 0;
}

void ProjectileSystem::Update()
{
	for (int i = 0; i < m_cells.size(); i++)
	{
		auto& [x, y, life] = m_cells.at(i);

		life -= iw::DeltaTime();
		if (life < 0.f)
		{
			sand->m_world->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::EMPTY));
			m_cells.at(i) = m_cells.back(); m_cells.pop_back(); i--;
		}
	}

	Space->Query<Projectile>().Each([&](iw::EntityHandle, Projectile* p) 
	{
		if (p->Update)
		{
			auto [hit, hx, hy] = p->Update();

			if (p->OnHit && hit) 
			{
				p->OnHit(hx, hy);
			}
		}
	});
}

float time__ = 0;

void ProjectileSystem::FixedUpdate()
{
	Space->Query<Projectile>().Each([&](iw::EntityHandle, Projectile* p) 
	{
		if (p->FixedUpdate) p->FixedUpdate();
	});

	time__ += iw::FixedTime();

	if (iw::Keyboard::KeyDown(iw::G) && time__ > .05)
	{
		time__ = 0;

		float x = iw::Mouse::ScreenPos().x() / Renderer->Width();
		float y = iw::Mouse::ScreenPos().y() / Renderer->Height();

		auto [w, h] = sand->GetSandTexSize2();

		float offX = iw::randf() * 1;
		float offY = iw::randf() * 1;

		glm::vec2 vel(x * 2 - 1, y * 2 - 1);
		vel = glm::normalize(vel) * 44.f;
		
		MakeBullet(w, h, vel.x + offX, -vel.y + offY, 1);
	}
}

bool ProjectileSystem::On(iw::ActionEvent& e)
{
	switch (e.Action) {
		case SPAWN_PROJECTILE:
		{
			SpawnProjectile_Event& event = e.as<SpawnProjectile_Event>();
			switch (event.Type) {
				case SpawnProjectile_Event::BULLET: 
				{
					MakeBullet(event.X, event.Y, event.dX, event.dY, 2);
					break;
				}
			}

			break;
		}
	}

	return false;
}

ProjectileSystem::ProjectileInfo ProjectileSystem::MakeProjectile(
	float  x, float  y,
	float dx, float dy,
	std::function<void(iw::SandChunk*, int, int)> placeCell)
{
	iw::Entity entity = Space->CreateEntity<
		iw::Transform, 
		iw::Rigidbody,
		iw::Circle,
		Projectile>();

	Projectile*    projectile = entity.Set<Projectile>();
	iw::Transform* transform  = entity.Set<iw::Transform>();
	iw::Circle*    collider   = entity.Set<iw::Circle>();
	iw::Rigidbody* rigidbody  = entity.Set<iw::Rigidbody>();

	transform->Position = glm::vec3(x, y, 0);

	rigidbody->SetTransform(transform);
	rigidbody->Velocity = glm::vec3(dx, dy, 0);
	rigidbody->Collider = collider;
	rigidbody->IsTrigger = true;

	Physics->AddRigidbody(rigidbody);

	auto getpos = [=]()
	{
		iw::Rigidbody* r = entity.Find<iw::Rigidbody>();
		iw::Transform* t = entity.Find<iw::Transform>();

		float x = t->Position.x;
		float y = t->Position.y;
		int   z = floor(t->Position.z); // Z Index

		float dx = r->Velocity.x;
		float dy = r->Velocity.y;

		return std::tuple(x, y, z, dx, dy);
	};

	auto setpos = [=](float x, float y)
	{
		iw::Rigidbody* r = entity.Find<iw::Rigidbody>();
		iw::Transform& t = r->Transform;

		t.Position.x = x;
		t.Position.y = y;
	};

	auto setvel = [=](float x, float y)
	{
		iw::Rigidbody* r = entity.Find<iw::Rigidbody>();

		r->Velocity.x = x;
		r->Velocity.y = y;
	};

	auto randvel = [=](float amount, bool setForMe) 
	{
		iw::Rigidbody* r = entity.Find<iw::Rigidbody>();
		float dx = r->Velocity.x;
		float dy = r->Velocity.y;

		float length = sqrt(dx * dx + dy * dy);
		float angle = atan2(dy, dx);
		angle += ((iw::randf()+1) * .5f) * amount + .1f * amount;

		dx = cos(angle) * length;
		dy = sin(angle) * length;

		if (setForMe) {
			r->Velocity.x = dx;
			r->Velocity.y = dy;
		}

		return std::tuple(dx * iw::FixedTime(), dy * iw::FixedTime());
	};

	projectile->Update = [=]() 
	{
		auto [x, y, z, dx, dy] = getpos();
		int zIndex = z; // for packing nonsense

		bool hit = false;
		float hx, hy;

		sand->ForEachInLine(x, y, x + dx * iw::DeltaTime(), y + dy * iw::DeltaTime(), [&](
			float fpx, float fpy)
		{
			int px = floor(fpx);
			int py = floor(fpy);

			iw::SandChunk* chunk = sand->m_world->GetChunk(px, py);
			
			if (!chunk)
			{
				hit = true; // collision on no chunk
			}

			else {
				auto& [tile, index] = chunk->GetCell<iw::TileInfo>(px, py, iw::SandField::TILE_INFO);
				if (tile)
				{
					if (tile->m_zIndex == zIndex)
					{
						sand->EjectPixel(tile, index);
						hit = true;
						tile = nullptr;
						index = 0;
					}
				}

				else
				if (  !chunk->IsEmpty(px, py)
					&& chunk->GetCell(px, py).Type != iw::CellType::PROJECTILE)
				{
					hit = true;
				}

				placeCell(chunk, px, py);
			}

			if (hit) 
			{
				hx = fpx;
				hy = fpy;
				return true;
			}

			return false;
		});

		return std::tuple(hit, hx, hy);
	};

	return { entity, projectile, getpos, setpos, setvel, randvel };
}

#define UNPACK_INFO                               \
		iw::Entity entity      = info.entity;     \
		Projectile* projectile = info.projectile; \
		auto getpos            = info.getpos;     \
		auto setpos            = info.setpos;     \
		auto setvel            = info.setvel;     \
		auto randvel           = info.randvel;    \

iw::Entity ProjectileSystem::MakeBullet(
	float start_x, float start_y, 
	float vx, float vy, 
	int maxDepth, int depth)
{
	if (depth > maxDepth)
	{
		return {};
	}
	
	auto placeCell = [=](iw::SandChunk* chunk, int px, int py) {

		iw::Cell cell;
		cell.Type = iw::CellType::PROJECTILE;
		cell.Color = iw::Color::From255(255, 230, 66);

		float length = .0002;

		chunk->SetCell(px, py, cell);
		m_cells.emplace_back(px, py, length);
	};

	ProjectileInfo info = MakeProjectile(start_x, start_y, vx, vy, placeCell);
	UNPACK_INFO;

	projectile->OnHit = [=](float fhx, float fhy)
	{
		int hx = floor(fhx);
		int hy = floor(fhy);

		if (sand->m_world->InBounds(hx, hy))
		{
			float depthPercent = depth / float(maxDepth) * iw::Pi / 2.f + iw::Pi / 6.f; // needs a lil tuning to get right feel
			int splitCount = iw::randi(2) + 1;

			auto [x, y, z, dx, dy] = getpos();

			//MakeBullet(hx, hy, dx, dy);

			float speed = sqrt(dx*dx + dy*dy);

			dx += iw::randfs() * speed / 5;
			dy += iw::randfs() * speed / 5;

			Bus->push<SpawnProjectile_Event>(fhx, fhy, dx, dy, SpawnProjectile_Event::BULLET);

		}

		//else {
			Space->QueueEntity(entity.Handle, iw::func_Destroy);
		//}
	};

	return entity;
}

void ProjectileSystem::MakeExplosion(
	int x, int y,
	int r)
{
	for (int px = x - r; px < x + r; px++)
	for (int py = y - r; py < y + r; py++)
	{
		if (iw::SandChunk* chunk = sand->m_world->GetChunk(px, py))
		{
			auto[tile, index] = chunk->GetCell<iw::TileInfo>(px, py, iw::SandField::TILE_INFO);

			if (tile)
			{
				sand->EjectPixel(tile, index);
			}

			chunk->SetCell(px, py, iw::Cell::GetDefault(iw::CellType::EMPTY));
		}
	}
}
