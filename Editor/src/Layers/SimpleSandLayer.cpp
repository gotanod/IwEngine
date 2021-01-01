#include "Layers/SimpleSandLayer.h"

#include "Sand/SandUpdateSystem.h"

namespace iw {
	SimpleSandLayer::SimpleSandLayer()
		: Layer("Simple Sand")
		, m_world(1920, 1080, 2) // screen width and height
	{
		srand(time(nullptr));
	}

	int SimpleSandLayer::Initialize() {
		// Setup values for cells

		_EMPTY = {
			CellType::EMPTY,
			CellProperties::NONE,
			iw::Color::From255(0, 0, 0, 0)
		};

		_SAND = { 
			CellType::SAND,
			CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE,
			iw::Color::From255(235, 200, 175)
		};

		_WATER = {
			CellType::WATER,
			CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE,
			iw::Color::From255(175, 200, 235)
		};

		_ROCK = {
			CellType::ROCK,
			CellProperties::NONE,
			iw::Color::From255(200, 200, 200)
		};

		Tile ship = {
			{
				              {2,6},
				       {1,5}, {2,5}, 
				{0,4}, {1,4}, {2,4}, {3,4},
				{0,3}, {1,3}, {2,3}, {3,3},
				{0,2},               {3,2},
				{0,1},               {3,1},
				{0,0},               {3,0},
			},
			{
							  _ROCK,
					   _ROCK, _ROCK,
				_ROCK, _ROCK, _ROCK, _ROCK,
				_ROCK, _ROCK, _ROCK, _ROCK,
				_ROCK,               _ROCK,
				_ROCK,               _ROCK,
				_ROCK,               _ROCK,
			}
		};

		iw::Entity shipEntity = Space->CreateEntity<iw::Transform, Tile>();
		shipEntity.Set<iw::Transform>(200);
		shipEntity.Set<Tile>(ship);

		PushSystem<SandUpdateSystem>(m_world, shipEntity.Handle);

		return 0;
	}

	void SimpleSandLayer::PostUpdate() {
		vector2 pos = Mouse::ScreenPos() / m_world.m_scale;
		pos.y = Renderer->Height() - pos.y;

		if (Mouse::ButtonDown(iw::LMOUSE)) {
			Cell placeMe = _EMPTY;

			     if (Keyboard::KeyDown(iw::S)) placeMe = _SAND;
			else if (Keyboard::KeyDown(iw::W)) placeMe = _WATER;
			else if (Keyboard::KeyDown(iw::R)) placeMe = _ROCK;

			for (size_t x = pos.x; x < pos.x + 20; x++)
			for (size_t y = pos.y; y < pos.y + 20; y++) {
				if (!m_world.InBounds(x, y)) continue;
				
				m_world.SetCell(x, y, placeMe);
			}
		}

		//if (Keyboard::KeyDown(iw::LEFT))  m_tiles[0].X -= 1;
		//if (Keyboard::KeyDown(iw::RIGHT)) m_tiles[0].X += 1;
		//if (Keyboard::KeyDown(iw::UP))    m_tiles[0].Y += 1;
		//if (Keyboard::KeyDown(iw::DOWN))  m_tiles[0].Y -= 1;

		for (Tile& tile : m_tiles) {
			for (auto [x, y] : tile.Positions) {
				//x += tile.X;
				//y += tile.Y;

				if (m_world.InBounds(x, y)) {
					// Call collision callback

					// what happens if the cell is already full?
					m_world.SetCell(x, y, _ROCK);
				}
			}
		}

		// Update cells

		for (size_t x = 0; x < m_world.m_width;  x++)
		for (size_t y = 0; y < m_world.m_height; y++) {
			const Cell& cell = m_world.GetCell(x, y);

				 if (cell.Props & CellProperties::MOVE_DOWN		 && MoveDown	(x, y, cell)) {}
			else if (cell.Props & CellProperties::MOVE_DOWN_SIDE && MoveDownSide(x, y, cell)) {}
			else if (cell.Props & CellProperties::MOVE_SIDE		 && MoveSide	(x, y, cell)) {}
		}

		m_world.CommitCells();
		
		// Update the sand texture

		unsigned int* pixels = (unsigned int*)m_sandTexture->Colors();
		for (size_t i = 0; i < m_world.m_width * m_world.m_height; i++) {
			pixels[i] = m_world.GetCell(i).Color;
		}

		if (m_world.InBounds(pos.x, pos.y)) {
			pixels[m_world.GetIndex(pos.x, pos.y)] = iw::Color(1, 0, 0);
		}
		
		for (Tile& tile : m_tiles) {
			for (auto [x, y] : tile.Positions) {
				//x += tile.X;
				//y += tile.Y;

				if (m_world.InBounds(x, y)) {
					// Call collision callback, could be a differnt one or just a bool flag that says that something collidded with me instead of i collided with something

					// what happens if the cell is no longer part of the tile?
					m_world.SetCell(x, y, _EMPTY);
				}
			}
		}

		// Draw the sand to the screen

		m_sandTexture->Update(Renderer->Device);

		Renderer->BeginScene(MainScene);
			Renderer->DrawMesh(iw::Transform(), m_sandMesh);
		Renderer->EndScene();
	}

	bool SimpleSandLayer::MoveDown(
		size_t x, size_t y,
		const Cell& cell)
	{
		bool down = m_world.IsEmpty(x, y - 1);
		if (down) {
			m_world.MoveCell(x, y, x, y - 1);
		}

		return down;
	}
	
	bool SimpleSandLayer::MoveDownSide(
		size_t x, size_t y,
		const Cell& cell)
	{
		bool downLeft  = m_world.IsEmpty(x - 1, y - 1);
		bool downRight = m_world.IsEmpty(x + 1, y - 1);

		if (downLeft && downRight) {
			downLeft  = iw::randf() > 0;
			downRight = !downLeft;
		}

		if		(downLeft)	m_world.MoveCell(x, y, x - 1, y - 1);
		else if (downRight) m_world.MoveCell(x, y, x + 1, y - 1);

		return downLeft || downRight;
	}

	bool SimpleSandLayer::MoveSide(
		size_t x, size_t y,
		const Cell& cell)
	{
		bool left  = m_world.IsEmpty(x - 1, y);
		bool right = m_world.IsEmpty(x + 1, y);

		ShuffleIfTrue(left, right);

			 if	(left)  m_world.MoveCell(x, y, x - 1, y);
		else if (right)	m_world.MoveCell(x, y, x + 1, y);

		return left || right;
	}
}

