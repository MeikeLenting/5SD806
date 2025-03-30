// appstate.cpp

#include "appstate.hpp"

namespace sim
{
   AppState::AppState()
      : m_running(true)
      , m_editor(m_world)
   {
   }

   bool AppState::init(int width, int height)
   {
      m_texture = LoadTexture("data/CustomTiles.png");
      cursorTexture = LoadTexture("data/Cursor.png");
      
      m_world.init(width, height, &m_texture, &cursorTexture);
      m_editor.init();

      return true;
   }

   void AppState::shut()
   {
      m_editor.shut();
      m_world.shut();
      
      UnloadTexture(m_texture);
      m_texture = {};
   }

   bool AppState::update(float dt)
   {
      if (IsKeyReleased(KEY_ESCAPE)) {
         m_running = false;
      }

      if (IsKeyPressed(KEY_F1)) {
         if (m_mode == Mode::VIEW) {
            m_mode = Mode::EDIT;
         }
         else if (m_mode == Mode::EDIT) {
            m_mode = Mode::VIEW;
         }
      }

      if (m_mode == Mode::VIEW) {
         m_world.update(dt);
      }
      else if (m_mode == Mode::EDIT) {
         m_editor.update(dt);
      }

      return m_running;
   }

   void AppState::render() const
   {
      m_world.render();
      if (m_mode == Mode::EDIT) {
         m_editor.render();
      }

      if (m_mode == Mode::EDIT) {
         const int font_size = 40;
         const Color color = MAROON;
         const char *text = TextFormat("%s", "EditMode");
         const int text_width = MeasureText(text, font_size);
         const int text_x = (GetScreenWidth() - text_width) / 2;
         const int text_y = 8;
         DrawText(text, text_x + 1, text_y + 1, font_size, BLACK);
         DrawText(text, text_x    , text_y    , font_size, color);
      }
   }
}
