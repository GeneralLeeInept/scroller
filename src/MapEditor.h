#pragma once

#include "Texture.h"

struct MapData
{
	int m_width, m_height;	// in tiles
	vector<string> m_tileTextures;
	vector<int> m_tiles;
};

class System;

class MapEditor
{
public:
	MapEditor(const System& system);
	
	bool HandleEvent(SDL_Event& e);
	void Update();
	void Draw(SDL_Renderer* renderer);

private:
	void LoadResources(const System& system);
	void ClampCursor();

	int m_width, m_height;	// in tiles
	vector<TexturePtr> m_tileTextures;
	vector<string> m_tileTexturePaths;
	vector<int> m_tileTextureReferenceCounts;
	vector<vector<int>> m_mapData;
	int m_cameraX, m_cameraY;
	TexturePtr m_cursor;
	int m_cursorX, m_cursorY;
	int m_brush;
	bool m_paint;
};