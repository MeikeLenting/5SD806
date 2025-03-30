// editor.cpp

#include "editor.hpp"
#include "world.hpp"

namespace sim
{
	namespace editor
	{
		void set_ground_active(std::vector<Ground>& ground, const Point& coord, const Point& world_size)
		{
			const int index = coord.y * world_size.x + coord.x;
			if (!ground[index].is_walkable()) {
				ground[index].set_walkable(true);
			}
		}

		void set_ground_inactive(std::vector<Ground>& ground, const Point& coord, const Point& world_size)
		{
			const int index = coord.y * world_size.x + coord.x;
			if (ground[index].is_walkable()) {
				ground[index].set_walkable(false);
			}
		}

		void set_grass_active(std::vector<Grass>& grass, const Point& coord, const Point& world_size)
		{
			const int index = coord.y * world_size.x + coord.x;
			if (!grass[index].is_alive()) {
				const float age = GetRandomValue(0, 100) / 100.0f;
				grass[index].set_age(age);
			}
		}

		void set_grass_inactive(std::vector<Grass>& grass, const Point& coord, const Point& world_size)
		{
			const int index = coord.y * world_size.x + coord.x;
			if (grass[index].is_alive()) {
				grass[index].set_age(0.0f);
			}
		}
	} // !editor

	Editor::Editor(World& world)
		: m_world(world)
	{
	}

	void Editor::init()
	{
	}

	void Editor::shut()
	{
	}

	bool Editor::update(float dt)
	{
		const auto& world_bounds = m_world.m_world_bounds;
		const auto& world_offset = m_world.m_world_offset;
		const auto& world_size = m_world.m_world_size;
		const auto& tile_size = m_world.m_tile_size;

		if (IsKeyPressed(KEY_TAB))
		{
			int setting = (int)currentSettings; 
			setting += 1; 
			if (setting > 7)
			{
				setting = 0; 
			}
			currentSettings = (Settings)setting;
		}

		// note: hover tile info
		m_is_tile_valid = false;
		m_cursor = GetMousePosition();
		if (CheckCollisionPointRec(m_cursor.to_vec2(), world_bounds)) {
			const Point cursor_world_position = m_cursor - world_offset;
			const Point hover_coord = cursor_world_position / tile_size;

			if (hover_coord.x >= 0 && hover_coord.x < world_size.x) {
				if (hover_coord.y >= 0 && hover_coord.y < world_size.y) {
					m_is_tile_valid = true;
					m_tile_coord = hover_coord;
					m_tile_index = hover_coord.y * world_size.x + hover_coord.x;
				}
			}
		}

		// note: edit mode logic
		if (m_is_tile_valid) {
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
				editor::set_ground_active(m_world.m_ground, m_tile_coord, world_size);
				editor::set_grass_active(m_world.m_grass, m_tile_coord, world_size);
			}

			if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
				editor::set_ground_inactive(m_world.m_ground, m_tile_coord, world_size);
				editor::set_grass_inactive(m_world.m_grass, m_tile_coord, world_size);
			}
		}

		return true;
	}

	void Editor::render() const
	{
		const auto& world_bounds = m_world.m_world_bounds;
		const auto& world_offset = m_world.m_world_offset;
		const auto& world_size = m_world.m_world_size;
		const auto& tile_size = m_world.m_tile_size;

		// note: debug grid
		const Color color = ColorAlpha(RAYWHITE, 0.3f);
		for (int y = 0; y <= world_size.y; y++) {
			const int ty = y * tile_size.y;
			DrawLine((int)world_bounds.x,
				(int)world_bounds.y + ty,
				(int)(world_bounds.x + world_bounds.width),
				(int)world_bounds.y + ty,
				color);
		}
		for (int x = 0; x <= world_size.x; x++) {
			const int tx = x * tile_size.x;
			DrawLine((int)world_bounds.x + tx,
				(int)world_bounds.y,
				(int)world_bounds.x + tx,
				(int)(world_bounds.y + world_bounds.height),
				color);
		}

		// note: sheep debug info
		for (const auto& sheep : m_world.m_sheep) {
			if (!sheep.isAlive) {
				continue;
			}
			
			bool shouldShowPath = currentSettings == showAllPaths || currentSettings == showOnlySheepPath || currentSettings == showSheepAndHerderPath || currentSettings == showWolfAndSheepPath;
			if (!sheep.path.empty () && shouldShowPath)
			{
				Color sheepPathColour = {139,102,204,100};
				for (auto tile : sheep.path)
				{
					//Draw the path
					DrawRectangle (world_offset.x + tile.x * tile_size.x,
						world_offset.y + tile.y * tile_size.y,
						tile_size.x,
						tile_size.y,
						sheepPathColour);

				}
			}
			// note: render collider
			DrawCircleLinesV(sheep.m_position, sheep.m_radius, MAGENTA);

			// note: walking direction
			DrawLineV(sheep.m_position, sheep.m_position + sheep.m_direction * Sheep::WALKING_SPEED, BLACK);


			const int font_size = 10;
			const char* stateName = "Invalid";
			switch (sheep.currentState) {
			case sheep.Hungry:
				stateName = "Hungry";
				break;
			case sheep.Satiated:
				stateName = "Satiated";
				break;
			case sheep.Reproducing:
				stateName = "Reproducing";
				break;
			case sheep.Afraid:
				stateName = "Afraid";
				break;
			}
			const char* text = TextFormat("State: %s\nCan Reproduce: %s\nMated With: %s\nAge: %.2f\nAmount Grass Eaten: %.0f\nVelocity: %.1f\nHunted: %s",
				stateName,
				sheep.canReproduce ? "Yes" : "No",
				sheep.isMatedWith ? "Yes" : "No",
				sheep.age,
				sheep.amountGrassEaten,
				sheep.velocity,
				sheep.isBeingHunted ? "Yes" : "No");
			DrawText(text, (int)sheep.m_position.x + (int)sheep.m_radius, int(sheep.m_position.y - sheep.m_radius), font_size, BLACK);
			DrawText(text, (int)sheep.m_position.x - 1 + (int)sheep.m_radius, int(sheep.m_position.y - sheep.m_radius - 1), font_size, WHITE);
		}

		// note: Wolf Debug Info
		{
			bool shouldShowPath = currentSettings == showAllPaths || currentSettings == showOnlyWolfPath || currentSettings == showHerderAndWolfPath || currentSettings == showWolfAndSheepPath;
			if (!m_world.wolf.path.empty () && shouldShowPath)
			{
				Color wolfPathColour = {204,102,175,150};
				for (auto tile : m_world.wolf.path)
				{
					//Draw the path
					DrawRectangle (world_offset.x + tile.x * tile_size.x,
						world_offset.y + tile.y * tile_size.y,
						tile_size.x,
						tile_size.y,
						wolfPathColour);

				}
			}

			// note: render collider
			DrawCircleLinesV(m_world.wolf.m_position, m_world.wolf.m_radius, PINK);

			// note: walking direction
			DrawLineV(m_world.wolf.m_position, m_world.wolf.m_position + m_world.wolf.m_direction * Wolf::WALKING_SPEED, BLACK);

			const char* stateName = "Invalid";
			switch (m_world.wolf.currentState) {
			case m_world.wolf.Hungry:
				stateName = "Hungry";
				break;
			case m_world.wolf.Satiated:
				stateName = "Satiated";
				break;
			case m_world.wolf.Asleep:
				stateName = "Asleep";
				break;
			}

			const int font_size = 10;
			const char* text = TextFormat("State: %s\nHas a Target: %s\nAmount Sheep Eaten: %d\nVelocity: %.1f\nTime Asleep: %.2f",
				stateName,
				m_world.wolf.hasATarget ? "Yes" : "No",
				m_world.wolf.amountSheepEaten,
				m_world.wolf.velocity,
				m_world.wolf.timeAsleep);
			DrawText(text, (int)m_world.wolf.m_position.x + (int)m_world.wolf.m_radius, int(m_world.wolf.m_position.y - m_world.wolf.m_radius), font_size, BLACK);
			DrawText(text, (int)m_world.wolf.m_position.x - 1 + (int)m_world.wolf.m_radius, int(m_world.wolf.m_position.y - m_world.wolf.m_radius - 1), font_size, WHITE);
			
		}

		// note: herder debug info
		{
			bool shouldShowPath = currentSettings == showAllPaths || currentSettings == showOnlyHerderPath || currentSettings == showHerderAndWolfPath || currentSettings == showSheepAndHerderPath;
			if (!m_world.herder.path.empty () && shouldShowPath)
			{
				Color herderPathColour = {102,234,201,100};
				for (auto tile : m_world.herder.path)
				{
					//Draw the path
					DrawRectangle (world_offset.x + tile.x * tile_size.x,
						world_offset.y + tile.y * tile_size.y,
						tile_size.x,
						tile_size.y,
						herderPathColour);
				}
			}
			Vector2 drawPosition = m_world.herder.m_position + m_world.herder.m_origin  /2.f;
			// note: render collider
			DrawCircleLinesV (drawPosition, m_world.herder.m_radius, MAGENTA);

			// note: walking direction
			DrawLineV (drawPosition, drawPosition + m_world.herder.m_direction * Herder::WALKING_SPEED, BLACK);


			const int font_size = 10;
			const char* text = TextFormat ("Position: %0.f, %0.f\nIs Attacked: %s",
				m_world.herder.m_position.x, m_world.herder.m_position.y, 
				m_world.herder.isAttacked ? "Yes" : "No");
			DrawText (text, (int)drawPosition.x + (int)m_world.herder.m_radius, int (drawPosition.y - m_world.herder.m_radius), font_size, BLACK);
			DrawText (text, (int)drawPosition.x - 1 + (int)m_world.herder.m_radius, int (drawPosition.y - m_world.herder.m_radius - 1), font_size, WHITE);
		}


		// note: hover tile debug info
		if (m_is_tile_valid) {
			DrawRectangleLines(world_offset.x + m_tile_coord.x * tile_size.x,
				world_offset.y + m_tile_coord.y * tile_size.y,
				tile_size.x,
				tile_size.y,
				WHITE);

			const int font_size = 10;
			const int cursor_offset_x = 2;
			const int cursor_offset_y = 20;
			const int x = m_cursor.x + cursor_offset_x;
			const int y = m_cursor.y + cursor_offset_y;
			const auto& ground = m_world.m_ground[m_tile_index];
			const auto& grass = m_world.m_grass[m_tile_index];
			const char* stateName = "Invalid"; 
			switch (grass.currentState) {
			case grass.Growing:
				stateName = "Growing";
				break;
			case grass.Fertilised:
				stateName = "Fertilised";
				break;
			case grass.FullyGrown:
				stateName = "Fully Grown";
				break;
			case grass.Wilting:
				stateName = "Wilting";
				break;
			}

			const char* text = TextFormat("Coord: %d,%d\nIndex: %d\nSolid: %s\nAge: %.2f\nState: %s ",
				m_tile_coord.x,
				m_tile_coord.y,
				m_tile_index,
				ground.is_walkable() ? "true" : "false",
				grass.get_age(),
				stateName);
			DrawText(text, x, y, font_size, BLACK);
			DrawText(text, x - 1, y - 1, font_size, WHITE);
		}		

	}
}
