// world.hpp

#pragma once

#include "common.hpp"
#include "Ground.h"
#include "Grass.h"
#include "Herder.h"
#include "Sheep.h"
#include "Wolf.h"
#include "Manure.h"
#include "Tile.h"
#include "queue"
#include <stack>

namespace sim
{
	struct Compare;
	struct World {
		static constexpr int TILE_SIZE			= 32;
		static constexpr int TILE_PADDING_X		= 3;
		static constexpr int TILE_PADDING_Y		= 2;
		static constexpr int START_AMOUNT_SHEEP	= 5;

		static constexpr Rectangle CURSOR_NORMAL  = {0.f, 0.f, 16.f, 16.f};
		static constexpr Rectangle CURSOR_BLOCKED = {16.f, 16.f, 16.f, 16.f};

		World ();

		void init	(int width, int height, Texture* texture, Texture* cursorTexture);
		void shut	();
		bool update (float dt);
		void render () const;

		bool is_valid_coord		(const Point& coord) const;
		bool isSheepValid		(int sheepIndex) const;
		bool IsAnotherSheep		(const Sheep& sheep1, const Sheep& sheep2) const;
		bool is_walkable		(const Point& coord) const;
		bool has_grass_at		(const Point& coord) const;
		bool isGrassFullyGrown  (const Point& coord) const;

		Point position_to_tile_coord	(const Vector2& position) const;
		Vector2 tile_coord_to_position	(const Point& coord) const;
		
		void CalculateNeighbouringTiles (const Vector2& position, Point (&resultingTiles)[8]);
		
		bool IsGroundFertilised (const Point& coord) const;
		
		bool CanGrassBeEaten (const Vector2& position) const;
		void EatGrass		 (const Vector2& position);
		
		
		Ground& ReturnGroundAt (const Point& coord);
		Grass&	ReturnGrassAt  (const Point& coord);
		
		void  SpawnSheep (Vector2 position, bool randomise = false);

		int	  ReturnMatingSheep		(const Sheep& sheep);
		bool  canSheepCurrentlyMate (int sheepIndex) const;
		bool  isInRangeOfMating		(int sheepIndex, const Vector2& position) const;
		
		int		ReturnSheepToEat	();
		bool	CanSheepBeEaten		(int sheepIndex);
		void	EatSheep			(Sheep& sheep);
		bool	IsWolfNearby		(const Point& coord) const;
		bool	IsHerderNearby		(const Point& coord) const;
		bool	IsHerderTooClose	(const Point& coord) const;
		void	AttackHerder		();

		Point	getRandomTile (Vector2 startPosition, float range);
		
		void  SetSheepAsMate (Sheep& sheep);
		void  ResetSheepMate (Sheep& sheep);
		
		void  Defecate (const Vector2& position);
		
		void  Fertilise		(const Point& coord, const Point& nearbyTiles);
		void  Defertilise	(const Point& coord, const Point& nearbyTiles);

		bool	AStarPathFinding		(const Point& startNode, const Point& targetNode, std::vector<Point> &path);
		float	CalculateHeuristicValue (Point tile, Point targetNode);
		bool	ExploreNeighbours				(std::vector<bool>& searchedTiles, std::vector<Tile>& tiles, std::priority_queue<Tile, std::vector<Tile>, Compare>& frontier, const Point& nearbyTile, const Point& targetNode, const Point& searchStart);
		
		inline int GetIndex (const Point& coord) const;
		void	   GetPath  (std::vector<Point>& path, const std::vector<Tile>& tiles, const Point& targetNode);

		bool m_running = true;

		Texture* m_texture{};
		Texture* m_cursorTexture{};

		Point m_tile_size;
		Point m_world_size;
		Point m_world_offset;
		
		Rectangle m_world_bounds{};

		std::vector<Ground> m_ground;
		std::vector<Grass>	m_grass;
		std::vector<Sheep>	m_sheep;
		std::vector<Manure> allManure;
		
		Manure manure;
		Wolf wolf; 
		Herder herder;
	};
} // !sim
