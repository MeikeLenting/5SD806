// world_update.cpp

#include "world.hpp"

namespace sim
{
	template <typename T>
	void contain_within_bounds (T& entity, const Rectangle& bounds)
	{
		if (entity.m_position.x < bounds.x + entity.m_radius)
		{
			entity.m_position.x = bounds.x + entity.m_radius;
			entity.m_direction.x = -entity.m_direction.x;
		}
		if (entity.m_position.x > bounds.x + bounds.width - entity.m_radius)
		{
			entity.m_position.x = bounds.x + bounds.width - entity.m_radius;
			entity.m_direction.x = -entity.m_direction.x;
		}
		if (entity.m_position.y < bounds.y + entity.m_radius)
		{
			entity.m_position.y = bounds.y + entity.m_radius;
			entity.m_direction.y = -entity.m_direction.y;
		}
		if (entity.m_position.y > bounds.y + bounds.height - entity.m_radius)
		{
			entity.m_position.y = bounds.y + bounds.height - entity.m_radius;
			entity.m_direction.y = -entity.m_direction.y;
		}
	}

	bool World::update (float dt)
	{
		if (IsKeyReleased (KEY_ESCAPE))
		{
			m_running = false;
		}

		// todo: update grass
		for (auto& grass : m_grass)
		{
			grass.Update (dt);
		}


		// note: update sheep
		for (auto& sheep : m_sheep)
		{
			sheep.update (dt);
			contain_within_bounds (sheep, m_world_bounds);
		}

		// update manure
		for (auto& manures : allManure)
		{
			manures.update (dt);
		};


		// update wolf
		{
			wolf.update (dt);
			contain_within_bounds (wolf, m_world_bounds);
		}

		// update herder
		{
			herder.Update (dt);
		}


		return m_running;
	}
}
