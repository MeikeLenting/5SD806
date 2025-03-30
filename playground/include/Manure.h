//Manure.h

#pragma once

#include "common.hpp"

namespace sim {
	struct World;

	struct Manure {
		Manure () = default;

		static constexpr Rectangle SOURCE	= {16.0f, 16.0f, 16.0f, 16.0f};
		static constexpr float MAX_DURATION = 10.f;


		void set_position (const Vector2& position);
		void SetTileCoord (const Point& coord);

		void set_sprite_source	(const Rectangle& source);
		void SetSpriteOrigin	(const Vector2& originSprite);

		void SpawnManure	();
		void DespawnManure	();

		void Initiate	(Point& coord);
		void update		(float dt);
		void render		(const Texture& texture) const;

		Vector2   m_position{};
		Vector2   origin{};
		
		Rectangle m_source{};
		
		Point     tileCoord{};
		
		float     m_duration{};

		bool      manureExists	= false;
		bool      hasFertilised = false;

		World* world = nullptr;
	};
}
