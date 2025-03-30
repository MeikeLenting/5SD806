//Ground.cpp

#include "Ground.h"

namespace sim {
	bool Ground::is_walkable () const
	{
		return m_walkable;
	}

	void Ground::set_walkable (bool state)
	{
		m_walkable = state;
	}

	void Ground::set_tile_coord (const Point& coord)
	{
		m_tile_coord = coord;
	}

	void Ground::FertiliseGround () {
		fertilised = true;
		source = FERTILISED_GROUND_SOURCE;
	}

	void Ground::UnfertiliseGround () {
		fertilised = false;
		source = UNFERTILISED_GROUND_SOURCE;
	}
}