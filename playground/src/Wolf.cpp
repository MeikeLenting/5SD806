//Wolf.cpp

#include "Wolf.h"
#include "world.hpp"

namespace sim {
	void Wolf::set_position (const Vector2& position)
	{
		m_position = position;
	}

	void Wolf::set_direction (const Vector2& direction)
	{
		m_direction = direction;
	}

	void Wolf::set_radius (float radius)
	{
		m_radius = radius;
	}

	void Wolf::set_sprite_flip_x (bool state)
	{
		m_flip_x = state;
	}

	void Wolf::set_sprite_origin (const Vector2& origin)
	{
		m_origin = origin;
	}

	void Wolf::set_sprite_source (const Rectangle& source)
	{
		m_source = source;
	}

	void Wolf::SetTargetPosition (const Vector2& position)
	{
		targetPosition = position;
		hasATarget = true;
		Vector2 direction = Vector2Normalize (Vector2Subtract (targetPosition, m_position));
		set_direction (direction);
	}

	void Wolf::TraverseUsingPath (std::vector<Point>& pathToTraverse)
	{
		//Ensuring that the wolf has a path to travel
		if (pathToTraverse.empty ())
		{
			return;
		}

		Vector2 target = world->tile_coord_to_position (pathToTraverse[0]);
		target = {target.x + world->m_tile_size.x / 2.f, target.y + world->m_tile_size.y / 2.f};
		SetTargetPosition (target);

		//Erasing the beginning of the path if the wolf reached the tile
		if (Vector2Distance (m_position, world->tile_coord_to_position (pathToTraverse[0])) < m_radius)
		{
			pathToTraverse.erase (pathToTraverse.begin ());
		}

	}

	void Wolf::Spawn ()
	{
		set_position ({192.f, 210.f}); //Set the position of the wolf in front of the den
		SpawnWolfsDen ({160.f, 160.f});

		set_radius (19.f);
		set_sprite_source (HUNGRY_SOURCE);
		set_sprite_origin (Vector2{m_source.width, m_source.height} *0.5f);

		sleepingPosition = {wolfsDenPosition.x + m_source.width , (wolfsDenPosition.y + wolfsDenSource.height / 2.f) + 0.5f * m_source.height};
		timeBetweenEating = DELAY_BETWEEN_EATING;
		amountSheepEaten = 0;
		velocity = WALKING_SPEED;
		timeAsleep = 0.f;

		hasATarget = false;
		randomTargetTile = {-1, -1};

		senseTimer.timePassed = 0.f;
		thinkTimer.timePassed = 0.f;

		senseTimer.maxTime = .5f;
		thinkTimer.maxTime = .25f;
	}

	void Wolf::SpawnWolfsDen (const Vector2& position)
	{
		wolfsDenSource = WOLFS_DEN_SOURCE;
		wolfsDenPosition = position;
	}

	void Wolf::GoToDen ()
	{
		TraverseUsingPath (path);
		set_sprite_source (SATIATED_SOURCE);
	}

	void Wolf::HuntSheep (int sheepIndex)
	{
		if (!world->isSheepValid (sheepIndex))
		{
			return;
		}

		velocity = RUNNING_SPEED;
		TraverseUsingPath (path);
		set_sprite_source (HUNGRY_SOURCE);
	}


	void Wolf::EatSheep ()
	{
		world->EatSheep (world->m_sheep[sheepToHunt]);
		velocity = WALKING_SPEED;
		timeBetweenEating = 0.f;
		amountSheepEaten += 1;
		hasATarget = false;
		sheepToHunt = -1;

		if (amountSheepEaten >= AMOUNT_SHEEP_SATIATED)
		{
			currentState = Satiated;
		}
	}

	void Wolf::Sleep ()
	{
		currentState = Asleep;
		set_sprite_source (SLEEPING_SOURCE);
	}

	void Wolf::Wander ()
	{
		//If the path is empty, or the tile doesnt exist, stop wandering
		if (path.empty () || !world->is_valid_coord (randomTargetTile))
		{
			return;
		}
		TraverseUsingPath (path);
	}

	void Wolf::AttackHerder ()
	{
		world->AttackHerder ();
		velocity = WALKING_SPEED;
		timeBetweenEating = 0.f;
		hasATarget = false;
		sheepToHunt = -1;
		
		currentState = Satiated;
	}

	void Wolf::update (float dt)
	{
		senseTimer.Update (dt);
		if (senseTimer.IsDone ())
		{
			senseTimer.Reset ();
			Sense (currentState, dt);
		}

		thinkTimer.Update (dt);
		if (thinkTimer.IsDone ())
		{
			thinkTimer.Reset ();
			Think (currentState, dt);
		}

		Act (currentState, dt);

	}

	void Wolf::render (const Texture& texture) const
	{
		Rectangle src = m_source;
		float width = src.width;

		if (m_flip_x)
		{
			src.width = -src.width;
		}

		Rectangle dest = {m_position.x, m_position.y, width, src.height};
		Vector2 origin = m_origin;
		DrawTexturePro (texture, src, dest, origin, 0.0f, WHITE);
	}

	void Wolf::RenderWolfsDen (const Texture& texture) const
	{
		Rectangle src = wolfsDenSource;

		Rectangle dest = {wolfsDenPosition.x, wolfsDenPosition.y, src.width, src.height};
		Vector2 origin = {-9.f,0.f}; //Adjust origin so the base of the wolfs den lines up with the tiles, instead of it being based on the roof
		DrawTexturePro (texture, src, dest, origin, 0.0f, WHITE);
	}

	void Wolf::Sense (State& state, float dt)
	{
		switch (state)
		{
			case Hungry:
			{
				bool doesTileExist = world->is_valid_coord (randomTargetTile);
				bool hasReachedDestination = Vector2Distance (m_position, world->tile_coord_to_position (randomTargetTile)) < m_radius;

				//Making sure the wolf does not get stuck in the wall (since it is a circle collider and with corners it can be iffy)
				if (randomTargetTile.x == 0.f || randomTargetTile.y == 0.f || randomTargetTile.x == world->m_world_size.x || randomTargetTile.y == world->m_world_size.y)
				{
					hasReachedDestination = Vector2Distance (m_position, world->tile_coord_to_position (randomTargetTile)) < 2 * m_radius;
				}

				if (!doesTileExist || hasReachedDestination)
				{
					randomTargetTile = world->getRandomTile (m_position, MAX_WANDERING_DISTANCE);
				}


				//Generating a random tile away from the herder
				if (world->IsHerderNearby (world->position_to_tile_coord (m_position)))
				{
					sheepToHunt = -1;
					Vector2 wolfPosition = world->wolf.m_position;
					Vector2 min = {m_position.x - MAX_HUNTING_DISTANCE, m_position.y - MAX_HUNTING_DISTANCE};
					Vector2 max = {m_position.x + MAX_HUNTING_DISTANCE, m_position.y + MAX_HUNTING_DISTANCE};

					if (wolfPosition.x < m_position.x)
					{
						min.x = m_position.x;
					}
					else
					{
						max.x = m_position.x;
					}

					if (wolfPosition.y < m_position.y)
					{
						min.y = m_position.y;
					}
					else
					{
						max.y = wolfPosition.y;
					}


					int x = GetRandomValue (int (min.x), int (max.x));
					int y = GetRandomValue (int (min.y), int (max.y));

					//Ensuring the target tile is in the world space
					randomTargetTile = world->position_to_tile_coord (Vector2Clamp ({(float)x,(float)y}, {0.f,0.f}, world->tile_coord_to_position (world->m_world_size)));
				}

				//When the herder is too close, actually move towards the herder
				if (world->IsHerderTooClose (world->position_to_tile_coord (m_position)))
				{
					randomTargetTile = world->position_to_tile_coord (world->herder.m_position);
				}

				//Making sure the wolf only hunts when the herder is away and doesn't yet have a target
				if (sheepToHunt == -1 && !world->IsHerderNearby (world->position_to_tile_coord (m_position)))
				{
					sheepToHunt = world->ReturnSheepToEat ();
				};

				//Search a path if the sheep exists
				if (world->isSheepValid (sheepToHunt))
				{
					world->AStarPathFinding (world->position_to_tile_coord (m_position), world->position_to_tile_coord (world->m_sheep[sheepToHunt].m_position), path);
				}

				//If the sheep does not exist, searching a path to a random tile
				if (sheepToHunt == -1 && world->is_valid_coord (randomTargetTile))
				{
					world->AStarPathFinding (world->position_to_tile_coord (m_position), randomTargetTile, path);
				}

				break;
			}

			case Satiated:
			{
				//Search path to den
				world->AStarPathFinding (world->position_to_tile_coord (m_position), world->position_to_tile_coord (sleepingPosition), path);
				break;
			}

			case Asleep:
			{
				break;
			}
		}

	}

	void Wolf::Think (State& state, float dt)
	{
		switch (state)
		{
			case Hungry:
			{
				//Ensuring the wolf runs instead of walks when the herder is nearby
				if (world->IsHerderNearby (world->position_to_tile_coord (m_position)))
				{
					velocity = RUNNING_SPEED;
				}
				else
				{
					velocity = WALKING_SPEED;
				}

				//If the herder is too close attack it
				if (world->IsHerderTooClose(world->position_to_tile_coord (m_position)))
				{
					AttackHerder ();
					break;
				}

				//Only wander if the target tile is valid and there is no sheep target
				if (sheepToHunt == -1 && world->is_valid_coord (randomTargetTile))
				{
					Wander ();
				}

				//Ensuring the wolf does not go hunt sheep when having eaten recently
				if (timeBetweenEating < DELAY_BETWEEN_EATING)
				{
					break;
				}

				HuntSheep (sheepToHunt);

				if (world->CanSheepBeEaten (sheepToHunt))
				{
					EatSheep ();
				}
				break;
			}

			case Satiated:
			{
				GoToDen ();

				if (Vector2Distance (m_position, sleepingPosition) < m_radius)
				{
					Sleep ();
				}
				break;
			}

			case Asleep:
			{
				set_sprite_source (SLEEPING_SOURCE);
				if (timeAsleep >= SLEEP_TIME)
				{
					timeAsleep = 0.f;
					amountSheepEaten = 0;
					set_sprite_source (HUNGRY_SOURCE);
					hasATarget = false;
					currentState = Hungry;
				}
				break;
			}
		}

	}

	void Wolf::Act (State& state, float dt)
	{
		switch (state)
		{
			case Hungry:
			{
				timeBetweenEating += dt;
				m_position += m_direction * velocity * dt;
				m_flip_x = m_direction.x > 0.0f ? true : false;
				break;
			}

			case Satiated:
			{
				m_position += m_direction * velocity * dt;
				m_flip_x = m_direction.x > 0.0f ? true : false;
				break;
			}

			case Asleep:
			{
				timeAsleep += dt;
				break;
			}
		}
	}
}