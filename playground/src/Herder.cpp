//Herder.cpp

#include "Herder.h"
#include "world.hpp"

namespace sim {
	void Herder::set_position (const Vector2& position)
	{
		m_position = position;
	}


	void Herder::set_direction (const Vector2& direction)
	{
		m_direction = direction;
	}

	void Herder::set_radius (float radius)
	{
		m_radius = radius;
	}

	void Herder::set_sprite_flip_x (bool state)
	{
		m_flip_x = state;
	}

	void Herder::set_sprite_origin (const Vector2& origin)
	{
		m_origin = origin;
	}

	void Herder::set_sprite_source (const Rectangle& source)
	{
		m_source = source;
	}

	void Herder::Init ()
	{
		set_radius (25.f);
		set_sprite_source (NORMAL_SOURCE);

		Vector2 origin = Vector2{m_source.width / 3.f, m_source.height / 4.f};
		set_sprite_origin (origin);

		Vector2 position = world->tile_coord_to_position ({21,21});
		set_position (position);
	}

	void Herder::Update (float dt)
	{
		//Stunning the herder when attacked
		if (isAttacked)
		{
			timeSinceAttack += dt;
		}

		if (timeSinceAttack >= STUN_TIME_ATTACKED)
		{
			isAttacked = false;
			timeSinceAttack = 0.f;
		}

		//Ensure that the player can't move the herder when attacked
		if (isAttacked)
		{
			return;
		}

		//Set target
		if (IsMouseButtonDown (MOUSE_BUTTON_LEFT))
		{
			targetCoord = world->position_to_tile_coord (GetMousePosition ());
		}

		//Search path to target
		if (world->is_valid_coord (targetCoord))
		{
			world->AStarPathFinding (world->position_to_tile_coord (m_position), targetCoord, path);
		}

		TraverseUsingPath (path);

		//Making sure the herder stops after having reached the destination
		bool hasReachedDestination = Vector2Distance (m_position, world->tile_coord_to_position (targetCoord)) < m_radius;
		if (world->is_valid_coord (targetCoord) && !hasReachedDestination)
		{
			m_position += m_direction * velocity * dt;
			m_flip_x = m_direction.x > 0.0f ? true : false;
		}
	}

	void Herder::Render (const Texture& texture) const {
		Rectangle src = m_source;
		float width = src.width;

		if (m_flip_x)
		{
			src.width = -src.width;
		}

		//Draw the frame showcasing which tile is the target
		bool hasReachedDestination = Vector2Distance (m_position, world->tile_coord_to_position (targetCoord)) < m_radius;
		if (world->is_valid_coord (targetCoord) && !hasReachedDestination)
		{
			Rectangle destination = {world->tile_coord_to_position (targetCoord).x,world->tile_coord_to_position (targetCoord).y, 32.f, 32.f};
			DrawTexturePro (texture, TARGET_FRAME_SOURCE, destination, {0.f, 0.f}, 0.0f, WHITE);
		}

		Rectangle dest = {m_position.x, m_position.y, width, src.height};
		Vector2 origin = m_origin;
		DrawTexturePro (texture, src, dest, origin, 0.0f, WHITE);
	}

	void Herder::SetTargetPosition (const Vector2& position)
	{
		targetPosition = position;
		Vector2 direction = Vector2Normalize (Vector2Subtract (targetPosition, m_position));
		set_direction (direction);
	}

	void Herder::TraverseUsingPath (std::vector<Point>& pathToTraverse)
	{
		//Return if there is no path to traverse
		if (pathToTraverse.empty ())
		{
			return;
		}

		Vector2 target = world->tile_coord_to_position (pathToTraverse[0]);
		target = {target.x + world->m_tile_size.x / 2.f, target.y + world->m_tile_size.y / 2.f};
		SetTargetPosition (target);

		//Erase the beginning of the path if the destination has been reached
		if (Vector2Distance (m_position, world->tile_coord_to_position (pathToTraverse[0])) < m_radius)
		{
			pathToTraverse.erase (pathToTraverse.begin ());
		}

	}
}