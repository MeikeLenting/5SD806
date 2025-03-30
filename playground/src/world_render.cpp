// world_render.cpp

#include "world.hpp"

namespace sim
{
	void World::render () const
	{
		assert (m_texture);

		const Vector2 ZERO{};
		const Vector2 tile_size = m_tile_size.to_vec2 ();

		{ // note: render ground
			for (const Ground& ground : m_ground)
			{
				if (!ground.is_walkable ())
				{
					continue;
				}

				const Vector2 position = (m_world_offset + ground.m_tile_coord * m_tile_size).to_vec2 ();
				const Rectangle destination{position.x, position.y, tile_size.x, tile_size.y};
				DrawTexturePro (*m_texture, ground.source, destination, ZERO, 0.0f, WHITE);
			}
		}

		{ // note: render grass

			for (const Grass& tile : m_grass)
			{
				if (!tile.is_alive ())
				{
					continue;
				}

				const int index = int (tile.m_age * _countof (tile.sources));
				const Vector2 position = (m_world_offset + tile.m_tile_coord * m_tile_size).to_vec2 ();
				const Rectangle source = tile.sources[index];
				const Rectangle destination{position.x, position.y, tile_size.x, tile_size.y};
				DrawTexturePro (*m_texture, source, destination, ZERO, 0.0f, WHITE);
			}
		}

		// note: render manure
		for (const auto& manures : allManure)
		{
			if (!manures.manureExists)
			{
				continue;
			}
			manures.render (*m_texture);
		}

		// note: render wolfs den
		wolf.RenderWolfsDen (*m_texture);

		// note: render sheep
		for (const auto& sheep : m_sheep)
		{
			if (!sheep.isAlive)
			{
				continue;
			}
			sheep.render (*m_texture);
		}

		// note: render wolf
		wolf.render (*m_texture);

		// note: render herder
		herder.Render (*m_texture);

		// note: render cursor
		{
			Rectangle source = CURSOR_NORMAL;
			//If the herder is attacked, render the blocked cursor
			if (herder.isAttacked)
			{
				source = CURSOR_BLOCKED;
			}
			DrawTexturePro (*m_cursorTexture, source, {GetMousePosition ().x, GetMousePosition ().y, 32.f, 32.f}, {0.f,0.f}, 0.f, WHITE);
		}
	}
}
