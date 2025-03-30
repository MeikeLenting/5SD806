//Sheep.cpp

#include "Sheep.h"
#include "world.hpp"

namespace sim {
	void Sheep::set_position (const Vector2& position)
	{
		m_position = position;
	}

	void Sheep::set_direction (const Vector2& direction)
	{
		m_direction = direction;
	}

	void Sheep::set_radius (float radius)
	{
		m_radius = radius;
	}

	void Sheep::set_sprite_flip_x (bool state)
	{
		m_flip_x = state;
	}

	void Sheep::set_sprite_origin (const Vector2& origin)
	{
		m_origin = origin;
	}

	void Sheep::set_sprite_source (const Rectangle& source, bool isHunted)
	{
		if (!isHunted)
		{
			sourceBeforeHunted = m_source;
		}
		m_source = source;
	}

	void Sheep::SetTargetPosition (const Vector2& position)
	{
		targetPosition = position;
		Vector2 direction = Vector2Normalize (Vector2Subtract (targetPosition, m_position));
		set_direction (direction);
	}

	void Sheep::TraverseUsingPath (std::vector<Point>& pathToTraverse)
	{
		//If the path is empty the sheep can't traverse
		if (pathToTraverse.empty ())
		{
			return;
		}

		Vector2 target = world->tile_coord_to_position (pathToTraverse[0]);
		//Setting target to middle of the tile
		target = {target.x + world->m_tile_size.x / 2.f, target.y + world->m_tile_size.y / 2.f};
		SetTargetPosition (target);

		//Erase the first tile in the path, when the tile has been reached
		if (Vector2Distance (m_position, world->tile_coord_to_position (pathToTraverse[0])) < m_radius)
		{
			pathToTraverse.erase (pathToTraverse.begin ());
		}

	}

	void Sheep::EatGrass () {
		world->EatGrass (m_position);
		set_sprite_source (EATING_SOURCE);
		amountGrassEaten++;
		timeBetweenEating = 0.0f;
		RegenerateHealth (HEALTH_REGENERATION);

		if (amountGrassEaten >= AMOUNT_GRASS_SATIATED)
		{
			canReproduce = true;
			timeSatiated = 0.f;
			currentState = Satiated;
		}
	}

	void Sheep::Defecate () {
		world->Defecate (m_position);
		set_sprite_source (NORMAL_SOURCE);
		amountGrassEaten = 0.f;
		timeBetweenEating = 0.f;
		timeSatiated = 0.f;
		currentState = Hungry;
	}

	void Sheep::Wander ()
	{
		//Don't traverse when there is no path or the tile is not valid
		if (path.empty () || !world->is_valid_coord (randomTargetTile))
		{
			return;
		}

		TraverseUsingPath (path);
	}

	bool Sheep::CanSheepEat () const
	{
		if (timeBetweenEating >= DELAY_EATING)
		{
			return true;
		}
		return false;
	}

	void Sheep::TakeDamage (float amountDamage) {
		health -= amountDamage;
		if (health < 0.0f)
		{
			health = 0.f;
		}
	}

	void Sheep::RegenerateHealth (float amountRegenerate)
	{
		health += amountRegenerate;
		if (health >= MAX_HEALTH)
		{
			health = MAX_HEALTH;
		}
	}

	void Sheep::GoTowardsMate (Sheep& sheepMate)
	{
		set_sprite_source (REPRODUCTION_SOURCE);
		velocity = RUNNING_SPEED;
		TraverseUsingPath (path);
	}

	void Sheep::Reproduce () {

		world->ResetSheepMate (world->m_sheep[sheepToMate]);
		canReproduce = false;
		set_sprite_source (SATIATED_SOURCE);
		randomTargetTile = {-1, -1};
		currentState = Satiated;
		sheepToMate = -1;
		velocity = WALKING_SPEED;
		world->SpawnSheep (m_position);
	}

	void Sheep::RunAway ()
	{
		canReproduce = false;
		velocity = RUNNING_SPEED;
		set_sprite_source (AFRAID_SOURCE, true);
		isMatedWith = false;
		//Making sure if the sheep has a mate, that the other sheep does not get stuck mating
		if (world->isSheepValid (sheepToMate))
		{
			world->ResetSheepMate (world->m_sheep[sheepToMate]);
		}

		TraverseUsingPath (path);
	}


	void Sheep::KillSheep () {
		isAlive = false;
		age = 0.f;

		//Making sure if the sheep has a mate, that the other sheep does not get stuck in mating
		if (world->isSheepValid (sheepToMate))
		{
			world->ResetSheepMate (world->m_sheep[sheepToMate]);
		}
	}

	void Sheep::Initiate (const Vector2& position)
	{
		const float radius = 20.0f;
		const float target_distance = 70.0f;
		const Rectangle source = NORMAL_SOURCE;
		const Vector2 origin = Vector2{source.width, source.height} *0.5f;


		currentState = Hungry;
		health = 10.f;
		age = 0.f;

		isAlive = true;
		canReproduce = false;

		velocity = WALKING_SPEED;
		sourceBeforeHunted = source;

		senseTimer.timePassed = 0.f;
		thinkTimer.timePassed = 0.f;

		senseTimer.maxTime = .25f;
		thinkTimer.maxTime = .5f;

		set_position (position);
		set_radius (radius);
		set_sprite_origin (origin);
		set_sprite_source (source);
	}

	void Sheep::update (float dt)
	{
		if (!isAlive)
		{
			return;
		}

		if (health <= 0.0f)
		{
			KillSheep ();
			return;
		}

		if (isBeingHunted && currentState != Afraid)
		{
			randomTargetTile = {-1, -1};
			isMatedWith = false;
			currentState = Afraid;
		}

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

	void Sheep::render (const Texture& texture) const
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
	void Sheep::Sense (State& state, float dt)
	{
		switch (state)
		{
			case Hungry:
			{
				bool doesTileExist = world->is_valid_coord (randomTargetTile);
				bool hasReachedDestination = Vector2Distance (m_position, world->tile_coord_to_position (randomTargetTile)) < m_radius;

				//Making sure the sheep cannot get stuck in case the target tile is a border tile, since it is a circle collider
				if (randomTargetTile.x == 0.f || randomTargetTile.y == 0.f || randomTargetTile.x == world->m_world_size.x || randomTargetTile.y == world->m_world_size.y)
				{
					hasReachedDestination = Vector2Distance (m_position, world->tile_coord_to_position (randomTargetTile)) < 2 * m_radius;
				}

				//Generate a new target
				if (!doesTileExist || hasReachedDestination || !world->has_grass_at (randomTargetTile))
				{
					randomTargetTile = world->getRandomTile (m_position, GRASS_HUNTING_RANGE);
				}

				//Search for path
				if (world->is_valid_coord (randomTargetTile))
				{
					world->AStarPathFinding (world->position_to_tile_coord (m_position), randomTargetTile, path);
				}
				break;
			}
			case Satiated:
			{
				bool doesTileExist = world->is_valid_coord (randomTargetTile);
				bool hasReachedDestination = Vector2Distance (m_position, world->tile_coord_to_position (randomTargetTile)) < m_radius + 2.f;

				//Making sure the sheep cannot get stuck in case the target tile is a border tile, since it is a circle collider
				if (randomTargetTile.x == 0.f || randomTargetTile.y == 0.f || randomTargetTile.x == world->m_world_size.x || randomTargetTile.y == world->m_world_size.y)
				{
					hasReachedDestination = Vector2Distance (m_position, world->tile_coord_to_position (randomTargetTile)) < 2 * m_radius;
				}

				if (!doesTileExist || hasReachedDestination)
				{
					randomTargetTile = world->getRandomTile (m_position, GRASS_HUNTING_RANGE);
				}

				//Searching path
				if (world->is_valid_coord (randomTargetTile))
				{
					world->AStarPathFinding (world->position_to_tile_coord (m_position), randomTargetTile, path);
				}

				break;
			}
			case Reproducing:
			{
				bool doesTileExist = world->is_valid_coord (randomTargetTile);
				bool hasReachedDestination = Vector2Distance (m_position, world->tile_coord_to_position (randomTargetTile)) < m_radius + 2.f;

				//Making sure the sheep cannot get stuck in case the target tile is a border tile, since it is a circle collider
				if (randomTargetTile.x == 0.f || randomTargetTile.y == 0.f || randomTargetTile.x == world->m_world_size.x || randomTargetTile.y == world->m_world_size.y)
				{
					hasReachedDestination = Vector2Distance (m_position, world->tile_coord_to_position (randomTargetTile)) < 2 * m_radius;
				}

				if (!doesTileExist || hasReachedDestination)
				{
					randomTargetTile = world->getRandomTile (m_position, GRASS_HUNTING_RANGE);
				}

				//Making sure the sheep stays in plays and does not perform unneccesary searching algorithms (since they are rather taxing)
				if (isMatedWith)
				{
					break;
				}

				if (sheepToMate == -1)
				{
					sheepToMate = world->ReturnMatingSheep (*this);
				}

				//Searching for path to sheep to mate
				if (world->isSheepValid (sheepToMate))
				{
					world->AStarPathFinding (world->position_to_tile_coord (m_position), world->position_to_tile_coord (world->m_sheep[sheepToMate].m_position), path);
				}

				//In case they have no sheep to mate, search for a path to a random tile
				if (world->is_valid_coord (randomTargetTile) && sheepToMate == -1)
				{
					world->AStarPathFinding (world->position_to_tile_coord (m_position), randomTargetTile, path);
				}
				break;
			}
			case Afraid:
			{
				bool doesTileExist = world->is_valid_coord (randomTargetTile);
				bool hasReachedDestination = Vector2Distance (m_position, world->tile_coord_to_position (randomTargetTile)) < m_radius + 2.f;

				//Making sure the sheep cannot get stuck in case the target tile is a border tile, since it is a circle collider
				if (randomTargetTile.x == 0.f || randomTargetTile.y == 0.f || randomTargetTile.x == world->m_world_size.x || randomTargetTile.y == world->m_world_size.y)
				{
					hasReachedDestination = Vector2Distance (m_position, world->tile_coord_to_position (randomTargetTile)) < 2 * m_radius;
				}

				//Generating a random tile specifically away from the wolf, based on where the wolf currently is
				if (!doesTileExist || hasReachedDestination)
				{
					Vector2 wolfPosition = world->wolf.m_position;
					Vector2 min = {m_position.x - FLEEING_RANGE, m_position.y - FLEEING_RANGE};
					Vector2 max = {m_position.x + FLEEING_RANGE, m_position.y + FLEEING_RANGE};

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

					//Making sure the tile is in world range
					randomTargetTile = world->position_to_tile_coord (Vector2Clamp ({(float)x,(float)y}, {0.f,0.f}, world->tile_coord_to_position (world->m_world_size)));
				}

				//Search path to the tile
				if (world->is_valid_coord (randomTargetTile))
				{
					world->AStarPathFinding (world->position_to_tile_coord (m_position), randomTargetTile, path);
				}

				break;
			}
		}
	}

	void Sheep::Think (State& state, float dt)
	{
		switch (state)
		{
			case Hungry:
			{
				if (world->is_valid_coord (randomTargetTile))
				{
					Wander ();
				}
				if (CanSheepEat () && world->CanGrassBeEaten (m_position))
				{
					EatGrass ();
				}

				if (timeBetweenEating > 0.3f)
				{
					set_sprite_source (NORMAL_SOURCE);
					if (timeBetweenEating >= TIME_BEFORE_DAMAGE)
					{
						TakeDamage (dt);
					}
				}
				break;
			}
			case Satiated:
			{
				if (world->is_valid_coord (randomTargetTile))
				{
					Wander ();
				}

				if (timeBetweenEating > 0.3f)
				{
					set_sprite_source (SATIATED_SOURCE);

					if (health >= MAX_HEALTH && canReproduce)
					{
						state = Reproducing;
						break;
					}
				}

				if (timeSatiated >= DELAY_DEFECATING)
				{
					Defecate ();
				}
				break;
			}
			case Reproducing:
			{
				set_sprite_source (REPRODUCTION_SOURCE);

				//Ensuring that if the sheep is mated with, it has an invalid target tile
				if (isMatedWith)
				{
					randomTargetTile = {-1, -1};
					break;
				}

				//Wandering around if the sheep the are trying to mate is invalid
				if (sheepToMate == -1)
				{
					if (world->is_valid_coord (randomTargetTile))
					{
						Wander ();
					}
					break;
				}

				if (world->canSheepCurrentlyMate (sheepToMate))
				{
					world->SetSheepAsMate (world->m_sheep[sheepToMate]);
					GoTowardsMate (world->m_sheep[sheepToMate]);
				}
				else
				{
					//Ensuring the sheep is reset if their mate happened to be killed or other
					if (!world->isSheepValid (sheepToMate))
					{
						sheepToMate = -1;
						set_sprite_source (SATIATED_SOURCE);
						canReproduce = true;
						isMatedWith = false;
						velocity = WALKING_SPEED;
						randomTargetTile = {-1, -1};
						state = Satiated;
					}
					sheepToMate = -1;
					velocity = WALKING_SPEED;
					randomTargetTile = {-1, -1};
					isMatedWith = false;
					break;
				}

				if (world->isInRangeOfMating (sheepToMate, m_position))
				{
					Reproduce ();
				}
				break;
			}
			case Afraid:
			{
				if (world->is_valid_coord (randomTargetTile))
				{
					RunAway ();
				}
				break;
			}
		}
	}

	void Sheep::Act (State& state, float dt)
	{
		switch (state)
		{
			case Hungry:
			{
				age += dt;
				timeBetweenEating += dt;
				m_position += m_direction * velocity * dt;
				m_flip_x = m_direction.x > 0.0f ? true : false;
				break;
			}
			case Satiated:
			{
				age += dt;
				timeBetweenEating += dt;
				timeSatiated += dt;
				m_position += m_direction * velocity * dt;
				m_flip_x = m_direction.x > 0.0f ? true : false;
				break;
			}
			case Reproducing:
			{
				age += dt;
				timeBetweenEating += dt;
				m_position += m_direction * velocity * dt;
				m_flip_x = m_direction.x > 0.0f ? true : false;
				break;
			}
			case Afraid:
			{
				age += dt;
				timeBetweenEating += dt;
				m_position += m_direction * velocity * dt;
				m_flip_x = m_direction.x > 0.0f ? true : false;
				break;
			}
		}
	}
}