#pragma once

#include "Texture.h"

class System;

class GameMap
{
public:
	enum TileLayer
	{
		kBackground,
		kPlayground,
		kForeground
	};

	GameMap() = default;
	GameMap(int numTilesX, int numTilesY);

	void Load(const string& path, const System& system);
	void Save(const string& path);

	int GetTile(TileLayer layer, int x, int y);
	void SetTile(TileLayer layer, int x, int y, TexturePtr texture);

	void SetBackdrop(TexturePtr texture);
	void SetParallaxLayer(int layer, float scrollScale, TexturePtr texture);

	void Draw(SDL_Renderer* renderer, int scrollX, int scrollY);

private:
	int TileIndex(int x, int y);
	int AddTexture(TexturePtr texture);

	vector<TexturePtr> m_textures;
	vector<int> m_tileMaps[3];
	vector<int> m_parallaxLayers;
	vector<float> m_parallaxScrollScales;
	int m_numTilesX;
	int m_numTilesY;
};