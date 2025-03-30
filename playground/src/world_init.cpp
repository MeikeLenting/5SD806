// world_init.cpp

#include "world.hpp"

namespace sim
{
	void World::init (int width, int height, Texture* texture, Texture* cursorTexture)
	{
		m_texture = texture;
		m_cursorTexture = cursorTexture;

		const int columns = (width / m_tile_size.x) - TILE_PADDING_X;
		const int rows = (height / m_tile_size.y) - TILE_PADDING_Y;
		const int start_x = (width - (columns * m_tile_size.x)) / 2;
		const int start_y = (height - (rows * m_tile_size.y)) / 2;

		// note: world settings
		m_world_size = {columns, rows};
		m_world_offset = {start_x, start_y};
		m_world_bounds = {
		   .x = float (m_world_offset.x),
		   .y = float (m_world_offset.y),
		   .width = float (columns * m_tile_size.x),
		   .height = float (rows * m_tile_size.y)
		};

		{ // note: initialize ground layer
			m_ground.resize (columns * rows);

			int ground_tile_index = 0;
			for (Ground& ground : m_ground)
			{
				const int x = (ground_tile_index % columns);
				const int y = (ground_tile_index / columns);
				ground_tile_index++;
				ground.source = ground.UNFERTILISED_GROUND_SOURCE;

				const Point tile_coord{x, y};
				ground.set_tile_coord (tile_coord);
				ground.set_walkable (true);
			}
		}

		{ // note: initialize grass layer
			m_grass.resize (columns * rows);

			int grass_tile_index = 0;
			for (Grass& grass : m_grass)
			{
				const int x = (grass_tile_index % columns);
				const int y = (grass_tile_index / columns);
				grass_tile_index++;

				const Point tile_coord{x, y};
				grass.set_tile_coord (tile_coord);

				// note: 50% chance to spawn
				if (GetRandomValue (0, 100) > 50)
				{
					const float age = (float)GetRandomValue (1, 100) / 100.0f;
					grass.set_age (age);
				}

				grass.world = this;
			}
		}

		{ // note: initialize sheep
			for (int i = 0; i < START_AMOUNT_SHEEP; i++)
			{
				SpawnSheep ({0,0}, true);
			}
		}

		{ // note: initialize wolf
			wolf.Spawn ();
			wolf.world = this;
		}

		{ // note: initialise manure
			allManure.resize (columns * rows);
			int manureTileIndex = 0;
			for (auto& manures : allManure)
			{
				manures.world = this;

				manureTileIndex++;
				const int x = (manureTileIndex % columns);
				const int y = (manureTileIndex / columns);
				Point coord = {x,y};
				manures.Initiate (coord);
			}
		}

		{ // note: initialize herder
			herder.world = this;
			herder.Init ();
		}
	}
	void World::shut ()
	{}
} // !sim
