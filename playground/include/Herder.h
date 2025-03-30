//Herder.h

#pragma once

#include "common.hpp"

namespace sim {
	struct World;

	struct Herder {
		Herder () = default;

		static constexpr float WALKING_SPEED = 70.f;
		static constexpr float STUN_TIME_ATTACKED = 4.f;
		static constexpr Rectangle NORMAL_SOURCE = {112.f, 56.f, 48.f, 41.f};
		static constexpr Rectangle TARGET_FRAME_SOURCE = {32.f, 16.f, 16.f, 16.f};

		void set_position	(const Vector2& position);
		void set_direction	(const Vector2& direction);
		void set_radius		(float radius);

		void SetTargetPosition (const Vector2& position);
		void TraverseUsingPath (std::vector<Point>& pathToTraverse);

		void set_sprite_flip_x (bool state);
		void set_sprite_origin (const Vector2& origin);
		void set_sprite_source (const Rectangle& source);

		void Init	();
		void Update (float dt);
		void Render (const Texture& texture) const;

		std::vector<Point>path;

		Vector2   m_position{};
		Vector2   m_direction{};
		Vector2   targetPosition{};
		Vector2   m_origin{};

		Point	  targetCoord = {-1, -1};

		Rectangle m_source{};

		float     m_radius{};
		float     velocity = WALKING_SPEED;
		float     timeSinceAttack = 0.f;

		bool      m_flip_x{};
		bool	  isAttacked = false;

		World* world = nullptr;
	};
}


