// editor.hpp

#pragma once

#include "common.hpp"

namespace sim
{
	struct World;

	struct Editor {
		enum Settings {
			showAllPaths,
			dontShowPaths,
			showOnlyWolfPath,
			showOnlySheepPath,
			showOnlyHerderPath,
			showWolfAndSheepPath,
			showSheepAndHerderPath,
			showHerderAndWolfPath,
		};


		Editor (World& world);

		void init ();
		void shut ();
		bool update (float dt);
		void render () const;

		World& m_world;

		Point m_cursor;
		Point m_tile_coord;

		bool m_is_tile_valid{};
		int m_tile_index{};

		Settings currentSettings = showAllPaths;
	};
}
