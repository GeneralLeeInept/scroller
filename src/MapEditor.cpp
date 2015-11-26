#include "stdafx.h"

#include "MapEditor.h"
#include "System.h"

#define TILESIZE 64
#define STATUSHEIGHT 256
#define SCREENTILESX (1280 / 64)
#define SCREENTILESY ((720 + 63) / 64)
#define MAXCURSORX (SCREENTILESX - 1)
#define MAXCURSORY (SCREENTILESY - 1)
#define MAPWIDTH (SCREENTILESX * 8)
#define MAPHEIGHT (SCREENTILESY * 8)
#define MAXSCROLLX (MAPWIDTH - SCREENTILESX)
#define MAXSCROLLY (MAPHEIGHT - SCREENTILESY)
#define OFFSCREENTILES 16

static const SDL_Rect s_offscreenRects[4] =
{
	{ 0, 0, OFFSCREENTILES, MAPHEIGHT },
	{ MAPWIDTH - OFFSCREENTILES, 0, OFFSCREENTILES, MAPHEIGHT },
	{ OFFSCREENTILES, 0, MAPWIDTH - 2 * OFFSCREENTILES, OFFSCREENTILES },
	{ OFFSCREENTILES, MAPHEIGHT - OFFSCREENTILES, MAPWIDTH - 2 * OFFSCREENTILES, OFFSCREENTILES }
};

MapEditor::MapEditor(const System& system)
	: m_mapData(MAPWIDTH, MAPHEIGHT)
	, m_system(system)
{
	LoadResources(system);
}

bool MapEditor::HandleEvent(SDL_Event& e)
{
	switch (e.type)
	{
		case SDL_MOUSEMOTION:
		{
			SDL_MouseMotionEvent& me = reinterpret_cast<SDL_MouseMotionEvent&>(e);
			m_cursorX = me.x / TILESIZE;
			m_cursorY = me.y / TILESIZE;
			return true;
		}

		case SDL_KEYUP:
		{
			SDL_KeyboardEvent& ke = reinterpret_cast<SDL_KeyboardEvent&>(e);

			switch (ke.keysym.scancode)
			{
				case SDL_SCANCODE_LEFTBRACKET:
				{
					--m_brush;
					return true;
				}

				case SDL_SCANCODE_RIGHTBRACKET:
				{
					++m_brush;
					return true;
				}

				case SDL_SCANCODE_GRAVE:
				{
					m_drawStatus = !m_drawStatus;
					return true;
				}

				case SDL_SCANCODE_F1:
				{
					m_activeLayer = GameMap::kBackground;
					return true;
				}

				case SDL_SCANCODE_F2:
				{
					m_activeLayer = GameMap::kPlayground;
					return true;
				}

				case SDL_SCANCODE_F3:
				{
					m_activeLayer = GameMap::kForeground;
					return true;
				}

				case SDL_SCANCODE_S:
				{
					m_mapData.Save("maps/test.map");
					return true;
				}

				case SDL_SCANCODE_L:
				{
					m_mapData.Load("maps/test.map", m_system);
					return true;
				}
			}

			break;
		}

		case SDL_KEYDOWN:
		{
			SDL_KeyboardEvent& ke = reinterpret_cast<SDL_KeyboardEvent&>(e);

			switch (ke.keysym.scancode)
			{
				case SDL_SCANCODE_LEFT:
				{
					--m_scrollX;
					return true;
				}

				case SDL_SCANCODE_RIGHT:
				{
					++m_scrollX;
					return true;
				}

				case SDL_SCANCODE_UP:
				{
					--m_scrollY;
					return true;
				}

				case SDL_SCANCODE_DOWN:
				{
					++m_scrollY;
					return true;
				}
			}

			break;
		}

		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
		{
			SDL_MouseButtonEvent& be = reinterpret_cast<SDL_MouseButtonEvent&>(e);

			if (be.button == SDL_BUTTON_LEFT)
			{
				m_paint = (be.type == SDL_MOUSEBUTTONDOWN);
			}

			if (be.button == SDL_BUTTON_RIGHT)
			{
				m_erase = (be.type == SDL_MOUSEBUTTONDOWN);
			}

			break;
		}

		default:
		{
			break;
		}
	}

	return false;
}

void MapEditor::Update(Uint32 ms)
{
	_CRT_UNUSED(ms);

	m_cursorX = max(0, min(m_cursorX, MAXCURSORX));
	m_cursorY = max(0, min(m_cursorY, MAXCURSORY));

	m_scrollX = max(0, min(m_scrollX, MAXSCROLLX));
	m_scrollY = max(0, min(m_scrollY, MAXSCROLLY));

	if (m_tileTextures.size() > 0)
	{
		m_brush = (m_brush < 0) ? m_tileTextures.size() - 1 : m_brush;
		m_brush = (m_brush > m_tileTextures.size() - 1) ? 0 : m_brush;
		m_brushTexture = m_tileTextures[m_brush];
	}

	if (m_paint || m_erase)
	{
		m_mapData.SetTile(m_activeLayer, m_cursorX + m_scrollX, m_cursorY + m_scrollY, m_paint ? m_brushTexture : nullptr);
	}
}

void MapEditor::Draw(SDL_Renderer* renderer)
{
	// Draw map
	m_mapData.Draw(renderer, m_scrollX * TILESIZE, m_scrollY * TILESIZE);

	// Draw overlays
	SDL_SetRenderDrawColor(renderer, 128, 0, 0, 128);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	for (int i = 0; i < 4; ++i)
	{
		SDL_Rect drawRect = s_offscreenRects[i];
		drawRect.x = (drawRect.x - m_scrollX) * TILESIZE;
		drawRect.y = (drawRect.y - m_scrollY) * TILESIZE;
		drawRect.w *= TILESIZE;
		drawRect.h *= TILESIZE;
		SDL_RenderFillRect(renderer, &drawRect);
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

	// Draw cursor
	SDL_Rect cursorRect = { m_cursorX * TILESIZE, m_cursorY * TILESIZE, TILESIZE, TILESIZE };

	SDL_RenderCopy(renderer, m_cursor, nullptr, &cursorRect);

	// Draw status area
	if (m_drawStatus)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_Rect statusRect = { 0, 720 - STATUSHEIGHT, 1280, STATUSHEIGHT };
		SDL_RenderFillRect(renderer, &statusRect);

		SDL_Rect brushRect = { statusRect.x + 16, statusRect.y + 16, 64, 64 };

		if (m_brushTexture)
		{
			SDL_RenderCopy(renderer, m_brushTexture, nullptr, &brushRect);
		}
		else
		{
			SDL_SetRenderDrawColor(renderer, 128, 255, 255, 255);
			SDL_RenderFillRect(renderer, &brushRect);
		}
	}
}

void MapEditor::LoadResources(const System& system)
{
	m_mapData.SetBackdrop(system.LoadTexture("backdrops/spooky.png"));
	m_cursor = system.LoadTexture("mapeditor/cursor.png");

	vector<string> textures = system.GetFilesInFolder("tiles/");

	for (auto path : textures)
	{
		try
		{
			m_tileTextures.push_back(system.LoadTexture(path));
		}
		catch (exception& e)
		{
			_CRT_UNUSED(e);
		}
	}
}
