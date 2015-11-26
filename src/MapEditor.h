#pragma once

#include "IGameState.h"

#include "GameMap.h"
#include "Texture.h"

struct MapData
{
	int m_width, m_height;  // in tiles
	vector<string> m_tileTextures;
	vector<int> m_tiles;
};

class System;

class MapEditor : public IGameState
{
public:
	MapEditor(const System& system);

	bool HandleEvent(SDL_Event& e);
	void Update(Uint32 ms);
	void Draw(SDL_Renderer* renderer);

private:
	void LoadResources(const System& system);

	const System& m_system;
	GameMap m_mapData;
	vector<TexturePtr> m_tileTextures;
	int m_scrollX = 0;
	int m_scrollY = 0;
	TexturePtr m_cursor = nullptr;
	int m_cursorX = 0;
	int m_cursorY = 0;
	int m_brush = -1;
	TexturePtr m_brushTexture = nullptr;
	bool m_paint = false;
	bool m_erase = false;
	bool m_drawStatus = true;
	GameMap::TileLayer m_activeLayer = GameMap::kPlayground;
};