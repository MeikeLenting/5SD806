//Timer.h

#pragma once

#include "common.hpp"

namespace sim {
	struct Timer {
		void Update (float dt);
		void Reset	();
		bool IsDone () const;

		float timePassed	= 0.0f;
		float maxTime		= 1.0f;
	};
}