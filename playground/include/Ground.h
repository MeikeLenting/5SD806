//Ground.h

#pragma once

#include "common.hpp"

namespace sim {
	struct Ground {

		Ground () = default;

		static constexpr Rectangle UNFERTILISED_GROUND_SOURCE	= {0.0f,  0.0f, 16.0f, 16.0f};
		static constexpr Rectangle FERTILISED_GROUND_SOURCE		= {0.0f, 16.0f, 16.0f, 16.0f};


		bool is_walkable	() const;
		void set_walkable	(bool state);
		void set_tile_coord (const Point& coord);

		void FertiliseGround	();
		void UnfertiliseGround	();

		Rectangle source = UNFERTILISED_GROUND_SOURCE;
		
		Point m_tile_coord;

		bool  m_walkable{};
		bool fertilised = false;
		
		float m_age{};

	};
}
