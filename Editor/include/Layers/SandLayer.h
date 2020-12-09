#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include <vector>

enum class CellType {
	EMPTY,

	SAND,
	WATER,
	ROCK,
	METAL,
	DEBRIS,

	LASER,
	BULLET
};

struct Cell {
	CellType Type;

	iw::Color Color;
	iw::vector2 Direction;
	float Life;
};

namespace iw {
	class SandLayer
		: public Layer
	{
	private:
		ref<Shader> shader;

		ref<Texture> texture;
		ref<RenderTarget> target;

		float m_scale;

		std::vector<Cell> m_cells;
		std::vector<Cell> m_swap;

		const Cell _EMPTY  = { CellType::EMPTY,  Color(0), 0, 0 };

		const Cell _SAND   = { CellType::SAND,   Color::From255(237, 201, 175), 0, 1 };
		const Cell _WATER  = { CellType::WATER,  Color::From255(175, 201, 237), 0, 0 };
		const Cell _ROCK   = { CellType::ROCK,   Color::From255(201, 201, 201), 0, 0 };
		const Cell _METAL  = { CellType::METAL,  Color::From255(230, 230, 230), 0, 100 };
		const Cell _DEBRIS = { CellType::DEBRIS, Color::From255(150, 150, 150), 0, 1 };

		const Cell _LASER  = { CellType::LASER,  Color::From255(255,   0,   0), 0, .06f };
		const Cell _BULLET = { CellType::BULLET, Color::From255(255,   255, 0), 0, .02f };

		iw::vector2 pMousePos, mousePos;
		float fireTimeout = 0;

		iw::vector2 movement;

		iw::Transform playerTransform; 
		iw::Transform enemyTransform;


		//   0123
		//0   ..
		//1  ....
		//2  ....
		//3  .  .
		//4  .  .
		vector2 playerCellLocations[14] {
			               vector2(1, 0), vector2(1, 0),
			vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
			vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
			vector2(0, 3),                               vector2(3, 3),
			vector2(0, 4),                               vector2(3, 4)
		};

	public:
		SandLayer();

		int Initialize() override;
		void PostUpdate() override;

		bool On(
			MouseMovedEvent& e) override;

		bool SandLayer::On(
			KeyEvent& e) override;
	private:
		void MoveLikeSand(
			size_t x, size_t y,
			const Cell& cell,
			const Cell& replacement);

		void MoveLikeWater(
			size_t x, size_t y,
			const Cell& cell,
			const Cell& replacement);

		std::pair<bool, iw::vector2> MoveForward(
			size_t x, size_t y,
			const Cell& cell,
			const Cell& replacement);

		void HitLikeBullet(
			size_t x, size_t y,
			const Cell& replacement);

		void HitLikeLaser(
			size_t x, size_t y,
			const Cell& replacement);

		std::vector<iw::vector2> FillLine(
			int x,  int y,
			int x2, int y2);

		size_t getCoords(
			const ref<Texture>& texture,
			size_t x, size_t y)
		{
			return x + y * texture->Width();
		}

		bool inBounds(
			const ref<Texture>& texture,
			size_t x, size_t y)
		{
			return  x < texture->Width()
				&& y < texture->Height();
		}

		bool isEmpty(
			const ref<Texture>& texture,
			size_t x, size_t y)
		{
			return  inBounds(texture, x, y)
				&& m_cells[getCoords(texture, x, y)].Type == _EMPTY.Type;
		}

		Cell* setCell(
			const ref<Texture>& texture,
			size_t x, size_t y,
			const Cell& cell)
		{
			if (inBounds(texture, x, y)) {
				return &(m_swap[getCoords(texture, x, y)] = cell);
			}

			return nullptr;
		}

		Cell& getCell(
			const ref<Texture>& texture,
			size_t x, size_t y)
		{
			return m_cells[getCoords(texture, x, y)];
		}
	};
}