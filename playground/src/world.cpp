// world.cpp

#include "world.hpp"

namespace sim
{
	//true means order is NOT correct, and elements should be swapped
	//false means order is correct, and elements don't swap
	struct Compare {
	
		bool operator()(const Tile& lhs, const Tile& rhs) {
			return (lhs.fValue > rhs.fValue);
		}
	};

	World::World ()
		: m_tile_size (TILE_SIZE, TILE_SIZE)
	{}

	bool World::is_valid_coord (const Point& coord) const
	{
		bool x_axis = coord.x >= 0 && coord.x < m_world_size.x;
		bool y_axis = coord.y >= 0 && coord.y < m_world_size.y;
		return x_axis && y_axis;
	}

	bool World::isSheepValid (int sheepIndex) const
	{
		if (sheepIndex < 0 || sheepIndex > m_sheep.size () - 1)
		{
			return false;
		}
		return true;
	}

	bool World::is_walkable (const Point& coord) const
	{
		if (!is_valid_coord (coord))
		{
			return false;
		}
		return m_ground[coord.y * m_world_size.x + coord.x].is_walkable ();
	}

	bool World::has_grass_at (const Point& coord) const
	{
		if (!is_valid_coord (coord))
		{
			return false;
		}

		return m_grass[coord.y * m_world_size.x + coord.x].is_alive ();
	}

	bool World::isGrassFullyGrown (const Point& coord) const {
		if (!is_valid_coord (coord))
		{
			return false;
		}
		return m_grass[coord.y * m_world_size.x + coord.x].isFullyGrown ();
	}

	Point World::position_to_tile_coord (const Vector2& position) const
	{
		Point result = position;
		result = result - m_world_offset;
		result = result / m_tile_size;
		return result;
	}

	Vector2 World::tile_coord_to_position (const Point& coord) const
	{
		Point pos = coord * m_tile_size + m_world_offset;
		return pos.to_vec2 ();
	}

	void World::CalculateNeighbouringTiles (const Vector2& position, Point (&resultingTiles)[8])
	{
		const Point neighbouringTiles[8] = {
			{ 0,-1 }, //North
			{ 1,-1 }, //North-East
			{ 1, 0 }, //East
			{ 1, 1 }, //South-East
			{ 0, 1 }, //South
			{-1, 1 }, //South-West
			{-1, 0 }, //West
			{-1,-1 }, //North-West
		};

		for (int i = 0; i < _countof (neighbouringTiles); i++)
		{
			Point result = position_to_tile_coord (position).operator+(neighbouringTiles[i]);
			resultingTiles[i] = result;
		}
	}

	bool World::IsGroundFertilised (const Point& coord) const {
		if (!is_valid_coord (coord))
		{
			return false;
		}
		return m_ground[coord.y * m_world_size.x + coord.x].fertilised;
	}

	bool World::CanGrassBeEaten (const Vector2& position) const
	{
		Point coord = position_to_tile_coord (position);
		const bool isGrassEdible = m_grass[coord.y * m_world_size.x + coord.x].isEdible;
		const bool isThereManure = allManure[coord.y * m_world_size.x + coord.x].manureExists;

		if (has_grass_at (coord) && isGrassEdible && !isThereManure)
		{
			return true;
		}

		return false;
	}

	void World::EatGrass (const Vector2& position)
	{
		ReturnGrassAt (position_to_tile_coord (position)).DespawnGrass ();
	}

	bool World::IsAnotherSheep (const Sheep& sheep1, const Sheep& sheep2) const
	{
		if (&sheep1 == &sheep2)
		{
			return false;
		}
		return true;
	}

	Ground& World::ReturnGroundAt (const Point& coord)
	{
		return m_ground[coord.y * m_world_size.x + coord.x];
	}

	Grass& World::ReturnGrassAt (const Point& coord)
	{
		return m_grass[coord.y * m_world_size.x + coord.x];
	}

	int World::ReturnMatingSheep (const Sheep& sheep)
	{
		int sheepIndex = -1;
		for (int i = 0; i < m_sheep.size (); i++)
		{
			const bool hasAMate = (m_sheep[i].isMatedWith == true) || (m_sheep[i].sheepToMate != -1);
			const bool isCurrentlyReproducing = (m_sheep[i].currentState == m_sheep[i].Reproducing) && hasAMate;
			const bool isAbleToReproduce = (m_sheep[i].currentState == m_sheep[i].Reproducing) && (m_sheep[i].isAlive == true);
			if (isCurrentlyReproducing || !isAbleToReproduce || !IsAnotherSheep (m_sheep[i], sheep))
			{
				continue;
			}

			sheepIndex = i;
			break;
		}
		return sheepIndex;
	}

	bool World::canSheepCurrentlyMate (int sheepIndex) const
	{
		if (!isSheepValid (sheepIndex))
		{
			return false;
		}

		if (!m_sheep[sheepIndex].isAlive || m_sheep[sheepIndex].currentState != m_sheep[sheepIndex].Reproducing)
		{
			return false;
		}

		return true;
	}

	bool World::isInRangeOfMating (int sheepIndex, const Vector2& position) const
	{
		const bool isInRangeOfMating = pow ((m_sheep[sheepIndex].m_position.x - position.x), 2.f) + pow ((m_sheep[sheepIndex].m_position.y - position.y), 2.f) <= pow ((2.f * m_sheep[sheepIndex].m_radius), 2.f) + 10.f;
		if (isInRangeOfMating)
		{
			return true;
		}
		return false;
	}

	int World::ReturnSheepToEat ()
	{
		int sheepIndex = -1;
		for (int i = 0; i < m_sheep.size (); i++)
		{
			//note: check if the sheep is a possible candidate to be eaten
			const bool isSheepNearby = Vector2Distance (wolf.m_position, m_sheep[i].m_position) <= wolf.MAX_HUNTING_DISTANCE;
			int distance = (int)Vector2Distance (wolf.m_position, m_sheep[i].m_position);

			if (isSheepNearby && m_sheep[i].isAlive)
			{
				sheepIndex = i;
				break;
			}
			m_sheep[i].set_sprite_source (m_sheep[i].sourceBeforeHunted);
			m_sheep[i].isBeingHunted = false;

			continue;
		}
		return sheepIndex;
	}

	bool World::CanSheepBeEaten (int sheepIndex)
	{
		if (!isSheepValid (sheepIndex))
		{
			return false;
		}

		Sheep& sheep = m_sheep[sheepIndex];
		sheep.isBeingHunted = true;

		const bool isSheepInEatingRange = Vector2Distance (wolf.m_position, sheep.m_position) <= wolf.m_radius + sheep.m_radius;
		if (sheep.isBeingHunted && isSheepInEatingRange)
		{
			return true;
		}
		return false;
	}

	void World::EatSheep (Sheep& sheep)
	{
		sheep.TakeDamage (10.0f);
		sheep.isBeingHunted = false;
	}

	bool World::IsWolfNearby (const Point& coord) const
	{
		if (!is_valid_coord (coord))
		{
			return true;
		}

		float distanceBetweenWolf = Vector2Distance (tile_coord_to_position(coord), wolf.m_position);

		if (distanceBetweenWolf <= 2 * m_sheep[0].m_radius)
		{
			return true;
		}

		return false;
	}

	bool World::IsHerderNearby (const Point& coord) const
	{
		if (!is_valid_coord (coord))
		{
			return true;
		}

		float distanceBetweenHerder = Vector2Distance (tile_coord_to_position (coord), herder.m_position + herder.m_origin);

		if (distanceBetweenHerder <= 5 * herder.m_radius)
		{
			return true;
		}

		return false;
	}

	bool World::IsHerderTooClose (const Point& coord) const
	{
		if (!is_valid_coord (coord))
		{
			return true;
		}
		
		float distanceBetweenHerder = Vector2Distance (tile_coord_to_position (coord), herder.m_position + herder.m_origin);

		if (distanceBetweenHerder <= 3 * herder.m_radius)
		{
			return true;
		}
		return false;
	}

	void World::AttackHerder ()
	{
		herder.isAttacked = true;
	}

	Point World::getRandomTile (Vector2 startPosition, float range)
	{
		int x = GetRandomValue (int (startPosition.x - range), int (startPosition.x + range));
		int y = GetRandomValue (int (startPosition.y - range), int (startPosition.y + range));

		Vector2 randomPosition = {(float)x,(float)y};

		randomPosition = Vector2Clamp (randomPosition, {0.f,0.f}, tile_coord_to_position (m_world_size));

		return position_to_tile_coord (randomPosition);
	}

	void World::SpawnSheep (Vector2 position, bool randomise)
	{
		if (randomise)
		{
			const int x = GetRandomValue (int (m_world_bounds.x), int (m_world_bounds.x + m_world_bounds.width));
			const int y = GetRandomValue (int (m_world_bounds.y), int (m_world_bounds.y + m_world_bounds.height));

			position = {(float)x, (float)y};
		}

		Sheep newSheep;
		newSheep.Initiate (position);
		newSheep.world = this;
		m_sheep.push_back (newSheep);
	}

	void World::SetSheepAsMate (Sheep& sheep)
	{
		sheep.isMatedWith = true;
		sheep.set_sprite_source (sheep.REPRODUCTION_SOURCE);
		sheep.SetTargetPosition (sheep.m_position);
		sheep.currentState = sheep.Reproducing;
	}

	void World::ResetSheepMate (Sheep& sheep)
	{
		sheep.velocity = sheep.WALKING_SPEED;
		sheep.set_sprite_source (sheep.SATIATED_SOURCE);
		sheep.randomTargetTile = {-1, -1};
		sheep.canReproduce = false;
		sheep.isMatedWith = false;
		sheep.currentState = sheep.Satiated;
	}

	void World::Defecate (const Vector2& position)
	{
		Point sheepPosition = position_to_tile_coord (position);
		allManure[sheepPosition.y * m_world_size.x + sheepPosition.x].SpawnManure ();
	}

	void World::Fertilise (const Point& coord, const Point& nearbyTiles)
	{
		m_grass[nearbyTiles.y * m_world_size.x + nearbyTiles.x].isFertilised = true;
		m_grass[coord.y * m_world_size.x + coord.x].isEdible = false;
		m_ground[nearbyTiles.y * m_world_size.x + nearbyTiles.x].FertiliseGround ();

	}

	void World::Defertilise (const Point& coord, const Point& nearbyTiles)
	{

		m_grass[nearbyTiles.y * m_world_size.x + nearbyTiles.x].isFertilised = false;
		m_grass[coord.y * m_world_size.x + coord.x].isEdible = true;
		m_ground[nearbyTiles.y * m_world_size.x + nearbyTiles.x].UnfertiliseGround ();
	}


	//Calculate the heuristic value using euclidean distance formula
	float World::CalculateHeuristicValue (Point startNode, Point targetNode)
	{
		return (float)(sqrt (pow (targetNode.x - startNode.x, 2) + pow (targetNode.y - startNode.y, 2)));
	}

	//Look at all neighbouring tiles, calculate the new g-score, f-value, and heuristic value, and updating the neighbours f-score if the new g-score is lower
	bool World::ExploreNeighbours (std::vector<bool>& searchedTiles, std::vector<Tile>& tiles, std::priority_queue<Tile, std::vector<Tile>, Compare>& frontier, const Point& nearbyTile, const Point& targetNode, const Point& searchStart)
	{
		//If the tile cant be walked to, make sure it  returns false, since it cannot be traversed
		if (!is_walkable (nearbyTile))
		{
			return false;
		}

		//Exit early if the neighbour is the target
		if (nearbyTile == targetNode)
		{
			tiles[GetIndex (nearbyTile)].parent = searchStart;
			return true; 
		}

		//Check if tile has been searched before, if yes, theres no path to be found from here
		if (searchedTiles[GetIndex (nearbyTile)] == true)
		{
			return false;
		}
		
		//Calculating the new values
		float gValue = tiles[GetIndex (searchStart)].gScore + 1.0f;
		float hValue = CalculateHeuristicValue (nearbyTile, targetNode);
		float fValue = gValue + hValue;

		//If the old f-score was worse than the new one, or if it is set to the default value, update the value
		if (tiles[GetIndex (nearbyTile)].fValue > fValue || tiles[GetIndex (nearbyTile)].fValue == FLT_MAX)
		{
			//Set the new tile to all the calculated values
			Tile newTile = {}; 
			newTile.fValue = fValue;
			newTile.gScore = gValue;
			newTile.heuristicValue = hValue;
			newTile.coord = nearbyTile;
			newTile.parent = searchStart;
			
			//Push it onto the frontier
			frontier.push (newTile);
			//Store the new values in the tiles vector in place of the old one
			tiles[GetIndex (nearbyTile)] = newTile;
		}
		
		//If the old f-score was better than the new one, return false
		return false;
	}

	//Get the index of a tile
	inline int World::GetIndex (const Point& coord) const
	{
		return (int)(coord.y * m_world_size.x + coord.x);
	}

	//Reconstruct the path 
	void World::GetPath (std::vector<Point>& path, const std::vector<Tile>& tiles, const Point& targetNode)
	{
		//Creating an empty stack, ensuring the last element added is the first to be removed
		std::stack<Point> pathStack = {};

		Point tile = targetNode; 

		//Start at the end of the path, moving backwards (through the parent) until the starting point is reached, where the tile is the same as the parent
		while (tile != tiles[GetIndex (tile)].parent)
		{
			pathStack.push (tile);
			tile = tiles[GetIndex (tile)].parent; 
		}

		//While the stack isn't empty, push the top of the stack onto the path vector, and remove this element from the stack, reconstructing the path in an accessible way
		while (!pathStack.empty ())
		{
			path.push_back (pathStack.top());
			pathStack.pop();
		}
	}

	bool World::AStarPathFinding (const Point& startNode, const Point& targetNode, std::vector<Point>& path)
	{
		//Ensuring the A* is not mixing with previously found paths
		path.clear ();

		//If the start node can't be walked on, or the target node/destination, stop searching since it is not possible
		if (!is_walkable (startNode) || !is_walkable (targetNode))
		{
			return false;
		}

		//Early return if the target is reached already
		if (startNode == targetNode)
		{
			return true; 
		}

		//Creating an empty priority queue, which will keep track of the tiles we have yet to explore, which is sorted based on the lowest F-value
		std::priority_queue<Tile, std::vector<Tile>, Compare> frontier;
		//The searched tiles vector keeps track of all the tiles that have been explored already, returning true for an element if it has been searched already
		std::vector<bool> searchedTiles; 
		//Ensuring the size of the vector is the same as the amount of tiles, and that each element is set to false by default
		searchedTiles.resize (m_ground.size (), false);
		
		//A vector containing all the tiles with their information such as G-scores, F-scores, H-scores
		std::vector<Tile> tiles; 

		//Initialising the tiles vector, ensuring the first tile is the start node
		tiles.resize (searchedTiles.size ());
		tiles[GetIndex (startNode)].fValue = 0.0f;
		tiles[GetIndex (startNode)].gScore = 0.0f;
		tiles[GetIndex (startNode)].heuristicValue = 0.0f;
		tiles[GetIndex (startNode)].coord = startNode;
		tiles[GetIndex (startNode)].parent = startNode;
		
		//Add the start tile to the frontier
		frontier.push (tiles[GetIndex (startNode)]);

		bool hasFoundPath = false; 

		//Only searching while the frontier is not empty
		while (!frontier.empty()) {
			
			//Keeping track of the first tile of the priority queue (also: the first one to search)
			Tile currentTile = frontier.top ();
			//Making sure to remove the first element
			frontier.pop ();

			//Since a priority queue is not able to be iterated over, there is a check to see if the current tile has already been searched. 
			//In the case it has been searched it gets skipped. 
			//Even though the same tile might be added to the frontier, the one with the lowest F-score (the most optimal one) will always be searched first
			if (searchedTiles[GetIndex (currentTile.coord)] == true)
			{
				continue;
			}
			//Setting the current tile to be searched
			searchedTiles[GetIndex (currentTile.coord)] = true;

			//Calculating all the surrounding tiles
			Point neighbours[8];
			CalculateNeighbouringTiles (tile_coord_to_position (currentTile.coord), neighbours);
			
			//Going through each neighbouring tile and exploring them
			for (auto nearbyTile : neighbours)
			{
				hasFoundPath = ExploreNeighbours (searchedTiles, tiles, frontier, nearbyTile, targetNode, currentTile.coord);

				//If a path has been found to the destination, reconstruct it. 
				if (hasFoundPath)
				{
					GetPath (path, tiles, targetNode);
					return true;
				}

			}
		}

		return false;
	}


	
}