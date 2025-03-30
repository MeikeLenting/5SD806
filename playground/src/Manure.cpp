//Manure.cpp

#include "Manure.h"
#include "world.hpp"

namespace sim {
	void Manure::set_position (const Vector2& position)
	{
		m_position = position;
	}

	void Manure::SetTileCoord (const Point& coord)
	{
		tileCoord = coord;
	}

	void Manure::set_sprite_source (const Rectangle& source)
	{
		m_source = source;
	}

	void Manure::SetSpriteOrigin (const Vector2& originSprite) {
		origin = originSprite;
	}

	void Manure::SpawnManure () {
		manureExists = true;
		hasFertilised = false;
		m_duration = 0.0f;
	}

	void Manure::DespawnManure () {
		manureExists = false;
	}

	void Manure::Initiate (Point& coord)
	{
		const Vector2 originSprite = {SOURCE.width / 2.f,SOURCE.height / 2.f};
		SetTileCoord ({coord.x,coord.y});
		set_position (world->tile_coord_to_position (coord));
		set_sprite_source (SOURCE);
		SetSpriteOrigin (originSprite);
		manureExists = false;
		hasFertilised = false;
		m_duration = 0.f;
	}

	//note: Since the Manure has very little states, which changes minimal things (mainly age and being alive) I chose to not further separate the sense-think-act for the manure entity. 
	void Manure::update (float dt)
	{
		//Sensing if the manure exists, based on that information it thinks (if it doesnt exist it should act), and acts (return)
		if (!manureExists)
		{
			return;
		}

		//Acting
		m_duration += dt;

		//Sensing the neighbouring tiles (could classify as thinking too)
		Point surroundingTiles[8];
		world->CalculateNeighbouringTiles (m_position, surroundingTiles);

		//Senses the duration and the max duration, and thinks if it should act
		if (m_duration >= MAX_DURATION)
		{
			//Thinks which tiles are nearby
			for (auto nearbyTiles : surroundingTiles)
			{
				//Senses if they are valid, and if they aren't it thinks it should act (continue)
				if (!world->is_valid_coord (nearbyTiles))
				{
					continue;
				}
				//Acting
				world->Defertilise (tileCoord, nearbyTiles);
			}
			//Acting
			DespawnManure ();
		}


		//Senses if it has already fertilised, if statement is thinking. return is acting
		if (hasFertilised)
		{
			return;
		}

		std::vector<Point> randomSurroundingTiles;

		//Thinks which tiles to add to the vector
		for (int i = 0; i < _countof (surroundingTiles); i++)
		{
			//Sensing which random value it is, thinking if it is true and thus if it should act
			if (GetRandomValue (0, 100) > 50)
			{
				//Acting
				randomSurroundingTiles.push_back (surroundingTiles[i]);
			}
		}

		for (auto nearbyTiles : randomSurroundingTiles)
		{
			//Senses if they are valid, and if they aren't it thinks it should act (continue)
			if (!world->is_valid_coord (nearbyTiles))
			{
				continue;
			}
			//Senses if the ground is fertilised, then thinks if it should act based on this information
			if (!world->IsGroundFertilised (nearbyTiles))
			{
				//Acting
				world->Fertilise (tileCoord, nearbyTiles);
			}
		}

		//Acting
		hasFertilised = true;
	}

	void Manure::render (const Texture& texture) const
	{
		Rectangle source = m_source;
		float width = source.width;
		Vector2 originManure = origin;
		Rectangle destination = {m_position.x + source.width, m_position.y + source.height, 16.f, 16.f};
		DrawTexturePro (texture, m_source, destination, originManure, 0.0f, WHITE);
	}
}