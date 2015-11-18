#include "stdafx.h"

#include <Windows.h>

#include "MapEditor.h"
#include "System.h"

#define TILESIZE 64
#define STATUSHEIGHT 256
#define MAXCURSORX ((1280 / 64) - 1)
#define MAXCURSORY (((720 - STATUSHEIGHT) / 64) - 1)

MapEditor::MapEditor(const System& system)
	: m_width(0)
	, m_height(0)
	, m_cameraX(0)
	, m_cameraY(0)
	, m_cursorX(0)
	, m_cursorY(0)
	, m_brush(0)
	, m_paint(false)
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
			if (ke.keysym.scancode == SDL_SCANCODE_LEFTBRACKET)
			{
				--m_brush;
			}
			else if (ke.keysym.scancode == SDL_SCANCODE_RIGHTBRACKET)
			{
				++m_brush;
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
			break;
		}
		default:
		{
			break;
		}
	}
	return false;
}

void MapEditor::Update()
{
	ClampCursor();
}

void MapEditor::Draw(SDL_Renderer* renderer)
{
	// Draw map

	// Draw cursor
	SDL_Rect cursorRect = { m_cursorX * TILESIZE, m_cursorY * TILESIZE, TILESIZE, TILESIZE };
	SDL_RenderCopy(renderer, m_cursor, nullptr, &cursorRect);

	// Draw status area
	SDL_Rect statusRect = { 0, 720 - STATUSHEIGHT, 1280, STATUSHEIGHT };
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &statusRect);
	
	SDL_Rect brushRect = { statusRect.x + 16, statusRect.y + 16, 64, 64 };
	TexturePtr brush;

	if (m_tileTextures.size() > 0)
	{
		m_brush = (m_brush < 0) ? m_tileTextures.size() - 1 : m_brush;
		m_brush = (m_brush > m_tileTextures.size() - 1) ? 0 : m_brush;
		brush = m_tileTextures[m_brush];
	}

	if (brush)
	{
		SDL_RenderCopy(renderer, brush, nullptr, &brushRect);
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, 128, 255, 255, 255);
		SDL_RenderFillRect(renderer, &brushRect);
	}

	if (m_paint && brush)
	{
		SDL_RenderCopy(renderer, brush, NULL, &cursorRect);
	}
}

vector<string> get_all_files_full_path_within_folder(string folder)
{
	vector<string> names;
	char search_path[200];
	sprintf_s(search_path, 200, "%s*.*", folder.c_str());
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// read all (real) files in current folder, delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				names.push_back(folder + fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}
void MapEditor::LoadResources(const System& system)
{
	m_cursor = system.LoadTexture("mapeditor/cursor.png");

	vector<string> textures = get_all_files_full_path_within_folder("tiles/");
	for (auto path : textures)
	{
		try
		{
			m_tileTextures.push_back(system.LoadTexture(path));
			m_tileTexturePaths.push_back(path);
			m_tileTextureReferenceCounts.push_back(0);
		}
		catch (exception& e)
		{

		}
	}
}

void MapEditor::ClampCursor()
{
	m_cursorX = max(0, min(m_cursorX, MAXCURSORX));
	m_cursorY = max(0, min(m_cursorY, MAXCURSORY));
}
