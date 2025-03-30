//Tile.h

#pragma once

#include "common.hpp"

namespace sim {
	struct Tile {
		float fValue = FLT_MAX; //Estimated Total Cost
		float gScore = FLT_MAX; //Actual Cost
		float heuristicValue = FLT_MAX; //Heuristic Value

		Point coord  =	{-1, -1};
		Point parent =	{-1, -1};

	};
}
