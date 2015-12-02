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

	static const Uint16 kEmptyTile = 0;

	GameMap() = default;
	GameMap(int numTilesX, int numTilesY);

	void Load(const string& path, const System& system);
	void Save(const string& path) const;

	int GetWidth() const;
	int GetHeight() const;

	Uint16 GetTile(TileLayer layer, int x, int y) const;
	void SetTile(TileLayer layer, int x, int y, TexturePtr texture);

	void SetBackdrop(TexturePtr texture);
	void SetParallaxLayer(int layer, float scrollScale, TexturePtr texture);

	void Draw(SDL_Renderer* renderer, int scrollX, int scrollY) const;

private:
	int TileIndex(int x, int y) const;
	Uint16 AddTexture(TexturePtr texture);
	TexturePtr GetTexture(Uint16 id) const;

	vector<TexturePtr> m_textures;
	vector<Uint16> m_tileMaps[3];
	vector<int> m_parallaxLayers;
	vector<float> m_parallaxScrollScales;
	int m_numTilesX;
	int m_numTilesY;
};