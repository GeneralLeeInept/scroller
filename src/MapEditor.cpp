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
	: m_width(MAPWIDTH)
	, m_height(MAPHEIGHT)
	, m_scrollX(0)
	, m_scrollY(0)
	, m_cursor(nullptr)
	, m_cursorX(0)
	, m_cursorY(0)
	, m_brush(-1)
	, m_brushTexture(nullptr)
	, m_paint(false)
	, m_erase(false)
	, m_drawStatus(true)
{
	LoadResources(system);

	m_mapData.resize(MAPHEIGHT * MAPWIDTH, -1);
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
		int& tile = TileAt(m_cursorX + m_scrollX, m_cursorY + m_scrollY);
		tile = m_paint ? m_brush : -1;
	}
}

void MapEditor::Draw(SDL_Renderer* renderer)
{
	SDL_Rect screenRect = { 0, 0, 1280, 720 };
	SDL_RenderCopy(renderer, m_backdrop, nullptr, &screenRect);

	// Draw map
	SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255);

	for (int y = 0; y < SCREENTILESY; ++y)
	{
		for (int x = 0; x < SCREENTILESX; ++x)
		{
			SDL_Rect tileRect = { x * TILESIZE, y * TILESIZE, TILESIZE, TILESIZE };
			int tile = TileAt(x + m_scrollX, y + m_scrollY);
			TexturePtr tileTexture = (tile < 0) ? nullptr : m_tileTextures[tile];

			if (tileTexture)
			{
				SDL_RenderCopy(renderer, tileTexture, nullptr, &tileRect);
			}
		}
	}

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
	m_cursor = system.LoadTexture("mapeditor/cursor.png");
	m_backdrop = system.LoadTexture("backdrops/spooky.png");

	vector<string> textures = system.GetFilesInFolder("tiles/");

	for (auto path : textures)
	{
		try
		{
			m_tileTextures.push_back(system.LoadTexture(path));
			m_tileTexturePaths.push_back(path);
		}
		catch (exception& e)
		{
			_CRT_UNUSED(e);
		}
	}
}

int& MapEditor::TileAt(int x, int y)
{
	if (x < 0 || x >= MAPWIDTH || y < 0 || y >= MAPHEIGHT)
	{
		throw exception("Map coordinates out of range");
	}

	return m_mapData.at(y * MAPWIDTH + x);
}
