//Timer.cpp

#include "Timer.h"

namespace sim {
	void Timer::Update (float dt) {
		timePassed += dt;
	}

	void Timer::Reset () {
		timePassed = 0.f;
	}
	bool Timer::IsDone () const
	{
		return timePassed >= maxTime;
	}
}