#pragma once

#include "Texture.h"

class GameMap
{
public:
	struct DiskHeader
	{
		char m_magic[4];
		int m_width;
		int m_height;
	};

	struct StringTable
	{
		int m_numStrings;
	};
	void load(const string& path);

private:
	struct ParallaxLayer
	{
		TexturePtr m_image;
		int m_maxScrollX, m_maxScrollY;
	};

	struct TileLayer
	{
		static const int EmptySpace = -1;
		vector<TexturePtr> m_tileTextures;
		vector<int> m_tiles;    // MapWidth * MapHeight 0 based indexes into m_tileTextures.
	};


	int m_width;
	int m_height;  // In tiles
	TexturePtr m_backdrop;
	vector<ParallaxLayer> m_parallaxLayers;
	vector<TileLayer> m_tileLayers;
};