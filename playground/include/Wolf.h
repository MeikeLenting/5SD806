//Wolf.h

#pragma once

#include "common.hpp"
#include "Timer.h"

namespace sim {
	struct World;

	struct Wolf {
		Wolf () = default;

		enum State {
			Hungry,
			Satiated,
			Asleep,
		};


		static constexpr float WALKING_SPEED			= 50.0f;
		static constexpr float RUNNING_SPEED			= 75.0f;
		static constexpr float MAX_HUNTING_DISTANCE		= 200.f;
		static constexpr float MAX_WANDERING_DISTANCE	= 300.f;
		static constexpr float SLEEP_TIME				= 10.f;
		static constexpr float DELAY_BETWEEN_EATING		= 3.f;

		static constexpr int AMOUNT_SHEEP_SATIATED = 3;

		static constexpr Rectangle HUNGRY_SOURCE	= {36.f, 194.f, 33.f, 28.f};
		static constexpr Rectangle SATIATED_SOURCE	= {10.f, 194.f, 23.f, 28.f};
		static constexpr Rectangle SLEEPING_SOURCE	= { 9.f, 229.f, 23.f, 22.f};
		static constexpr Rectangle WOLFS_DEN_SOURCE = {71.f, 183.f, 48.f, 41.f};


		void set_position	(const Vector2& position);
		void set_direction	(const Vector2& direction);
		void set_radius		(float radius);

		void set_sprite_flip_x (bool state);
		void set_sprite_origin (const Vector2& origin);
		void set_sprite_source (const Rectangle& source);

		void SetTargetPosition (const Vector2& position);
		void TraverseUsingPath (std::vector<Point>& pathToTraverse);

		void Spawn	 ();
		void GoToDen ();

		void HuntSheep  (int sheepIndex);
		void EatSheep	();
		void Sleep		();
		void Wander		();

		void AttackHerder ();

		void SpawnWolfsDen (const Vector2& position);

		void update (float dt);
		void render (const Texture& texture) const;
		
		void RenderWolfsDen (const Texture& texture) const;

		void Sense	(State& state, float dt);
		void Think	(State& state, float dt);
		void Act	(State& state, float dt);

		std::vector<Point> path;

		State     currentState = Hungry;

		Vector2   m_position{};
		Vector2   wolfsDenPosition{};
		Vector2   sleepingPosition{};
		Vector2   m_direction{};
		Vector2   targetPosition{};
		Vector2   m_origin{};

		Rectangle m_source{};
		Rectangle wolfsDenSource{};

		Point	  randomTargetTile = {-1,-1};

		float     m_radius{};
		float     velocity			= WALKING_SPEED;
		float     timeBetweenEating = 0.f;
		float     timeAsleep		= 0.f;
		
		bool	m_flip_x{};
		bool    hasATarget	= false;

		int		amountSheepEaten	= 0;
		int     sheepToHunt			= -1;
		
		Timer senseTimer;
		Timer thinkTimer;
		Timer actTimer;

		World* world = nullptr;

	};
}
