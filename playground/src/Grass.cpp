//Grass.cpp

#include "Grass.h"
#include "world.hpp"

namespace sim {
	bool Grass::is_alive () const
	{
		return m_age > 0.0f;
	}

	float Grass::get_age () const
	{
		return m_age;
	}

	void Grass::set_age (float age)
	{
		m_age = age;
	}

	void Grass::set_tile_coord (const Point& coord)
	{
		m_tile_coord = coord;
	}

	bool Grass::isFullyGrown () const {
		if (m_age >= 0.67f)
		{
			return true;
		}

		return false;
	}

	void Grass::SpawnGrass () {
		set_age (0.01f);
		hasSeedsAvailable = true;
		isEdible = true;
		currentState = Growing;
	}

	void Grass::DespawnGrass () {
		isEdible = false;
		set_age (0);
	}


	//note: Since the grass has very little states, which changes minimal things (mainly age) I chose to not further separate the sense-think-act for the grass entity. 
	void Grass::Update (float dt) {
		//is alive : sensing, if-statement: thinking, return: acting
		if (!is_alive ())
		{
			return;
		}

		//Senses which state it currently is in
		switch (currentState)
		{
			case Growing:
			{
				//Acting
				set_age (m_age + NORMAL_GROW_SPEED * dt);

				//Sensing if it is fully grown, if-statement: thinking
				if (isFullyGrown ())
				{
					//Acting
					currentState = FullyGrown;
				}

				//Sensing if it is fertilised, if-statement: thinking
				if (isFertilised)
				{
					//Acting
					currentState = Fertilised;
				}

				break;
			}

			case Fertilised:
			{
				//Acting
				set_age (m_age + FERTILISED_GROW_SPEED * dt);
				//Sensing if it is fully grown, if-statement: thinking
				if (isFullyGrown ())
				{
					//Acting
					currentState = FullyGrown;
				}

				//Sensing if it is not fertilised, if-statement: thinking
				if (!isFertilised)
				{
					//Acting
					currentState = Growing;
				}

				break;
			}

			case FullyGrown:
			{
				//Acting
				set_age (m_age + NORMAL_GROW_SPEED * dt);

				//Sensing if the age is above a threshold, if-statement: thinking
				if (m_age >= 0.84f)
				{
					//Acting
					currentState = Wilting;
				}

				//Sensing if it is has seeds available, if-statement: thinking, return: acting
				if (!hasSeedsAvailable)
				{
					return;
				}

				//Sensing the surrounding tiles (technically could classify as thinking as well)
				Point surroundingTiles[8];
				world->CalculateNeighbouringTiles (world->tile_coord_to_position (m_tile_coord), surroundingTiles);
				for (auto nearbyTiles : surroundingTiles)
				{
					//Thinking
					if (!world->is_valid_coord (nearbyTiles))
					{
						continue;
					}
					//Thinking based on the sense of ground being fertilised
					if (world->ReturnGroundAt (nearbyTiles).fertilised)
					{
						//Acting
						isFertilised = true;
					}
				}

				//Thinking which tiles will the grass spreads to
				std::vector<Point> randomSurroundingTiles;
				for (int i = 0; i < _countof (surroundingTiles); i++)
				{
					if (GetRandomValue (0, 100) > 50)
					{
						randomSurroundingTiles.push_back (surroundingTiles[i]);
					}
				}

				//Thinking which grass is available to spread to (based on the senses, is valid coord)
				for (auto nearbyTiles : randomSurroundingTiles)
				{
					if (!world->is_valid_coord (nearbyTiles))
					{
						continue;
					}
					//Thinking, based on the sense of has it grass
					if (!world->has_grass_at (nearbyTiles))
					{
						//Acting
						world->ReturnGrassAt (nearbyTiles).SpawnGrass ();
					}
				}

				//Act
				hasSeedsAvailable = false;

				break;
			}

			case Wilting:
			{
				//Act
				set_age (m_age + NORMAL_GROW_SPEED * dt);

				//Thinking, based on the information it senses from age
				if (m_age >= 1.0f)
				{
					//Acting
					DespawnGrass ();
				}

				break;
			}
		}
	}
}