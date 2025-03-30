//Grass.h

#pragma once

#include "common.hpp"

namespace sim {
	struct World;

	struct Grass {

		Grass () = default;

		enum State {
			Growing,
			Fertilised,
			FullyGrown,
			Wilting,
		};

		static constexpr float NORMAL_GROW_SPEED		= 0.01f;
		static constexpr float FERTILISED_GROW_SPEED	= 0.05f;

		static constexpr Rectangle sources[6] =
		{
		   { 16.0f, 0.0f, 16.0f, 16.0f }, // Seed
		   { 32.0f, 0.0f, 16.0f, 16.0f },
		   { 48.0f, 0.0f, 16.0f, 16.0f },
		   { 64.0f, 0.0f, 16.0f, 16.0f },
		   { 80.0f, 0.0f, 16.0f, 16.0f }, // Fully Grown
		   { 96.0f, 0.0f, 16.0f, 16.0f }, // Wilting
		};


		bool	is_alive ()	const;
		float	get_age () const;
		void	set_age (float age);
		void	set_tile_coord (const Point& coord);

		bool	isFullyGrown () const;

		void	SpawnGrass ();
		void	DespawnGrass ();
		void	Update (float dt);

		State currentState = Growing;
		Point m_tile_coord;
		float m_age{};

		bool hasSeedsAvailable	= true;
		bool isFertilised		= false;
		bool isEdible			= true;

		World* world = nullptr;
	};
}