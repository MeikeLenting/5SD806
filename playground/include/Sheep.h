//Sheep.h

#pragma once

#include "common.hpp"
#include "Timer.h"

namespace sim {
	struct World;
	struct Sheep {

		Sheep () = default;

		enum State {
			Hungry,
			Satiated,
			Reproducing,
			Afraid,
		};

		static constexpr float WALKING_SPEED			= 50.0f;
		static constexpr float RUNNING_SPEED			= 70.0f;
		static constexpr float MAX_HEALTH				= 10.f;
		static constexpr float AMOUNT_GRASS_SATIATED	= 3.f;
		
		static constexpr float DELAY_DEFECATING			= 3.0f;
		static constexpr float DELAY_EATING				= 2.0f;
		static constexpr float TIME_BEFORE_DAMAGE		= 5.0f;
		static constexpr float HEALTH_REGENERATION		= 2.0f;
		
		static constexpr float GRASS_HUNTING_RANGE		= 150.0f;
		static constexpr float FLEEING_RANGE			= 300.0f;

		static constexpr Rectangle NORMAL_SOURCE		= { 7.0f,  59.0f, 39.0f, 29.0f};
		static constexpr Rectangle EATING_SOURCE		= {53.0f,  59.0f, 43.0f, 29.0f};
		static constexpr Rectangle AFRAID_SOURCE		= { 3.0f, 101.0f, 42.0f, 37.0f};
		static constexpr Rectangle SATIATED_SOURCE		= {55.0f, 104.0f, 44.0f, 34.0f};
		static constexpr Rectangle REPRODUCTION_SOURCE	= {	4.0f, 145.0f, 40.0f, 36.0f};



		void set_position		(const Vector2& position);
		void set_direction		(const Vector2& direction);
		void set_radius			(float radius);
		void SetTargetPosition	(const Vector2& position);
		void TraverseUsingPath	(std::vector<Point>& pathToTraverse);

		void set_sprite_flip_x	(bool state);
		void set_sprite_origin	(const Vector2& origin);
		void set_sprite_source	(const Rectangle& source, bool isHunted = false);
		
		bool CanSheepEat	() const;
		void EatGrass		();
		void Defecate		();
		void Wander			();

		void KillSheep			();
		void TakeDamage			(float amountDamage);
		void RegenerateHealth	(float amountRegenerate);

		void GoTowardsMate	(Sheep& sheepMate);
		void Reproduce		();

		void RunAway ();

		void Initiate	(const Vector2& position);
		void update		(float dt);
		void render		(const Texture& texture) const;

		void Sense	(State& state, float dt);
		void Think	(State& state, float dt);
		void Act	(State& state, float dt);

		std::vector<Point> path;

		State     currentState = Hungry;

		Vector2   m_position{};
		Vector2   targetPosition{};
		Vector2   m_direction{};
		Vector2   m_origin{};

		Rectangle m_source{};
		Rectangle sourceBeforeHunted = NORMAL_SOURCE;

		Point	randomTargetTile = {-1,-1};

		float m_radius{};
		float age				= 0.f;
		float health			= MAX_HEALTH;
		float amountGrassEaten	= 0.f;
		float timeSatiated		= 0.0f;
		float timeBetweenEating	= 0.0f;
		float velocity			= WALKING_SPEED;

		bool m_flip_x{};
		bool isAlive		= true;
		bool canReproduce	= false;
		bool isBeingHunted	= false;
		bool isMatedWith	= false;
		
		int	sheepToMate = -1;

		Timer senseTimer;
		Timer thinkTimer;

		World* world = nullptr;
	};
}
