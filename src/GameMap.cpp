#include "stdafx.h"

#include "GameMap.h"
#include "System.h"

#define MAPMAGIC (('G' << 24) | ('M' << 16) | ('A' << 8) | 'P')
#define SCREENTILESX (1280 / 64)
#define SCREENTILESY ((720 + 63) / 64)

struct DiskHeader
{
	Uint32 m_magic; // 'G','M','A','P'
	Uint16 m_width; // In tiles
	Uint16 m_height; // In tiles
	Uint16 m_numParallaxLayers;
	Uint16 m_texturePathsSize;
};

struct DiskTile
{
	static const Uint16 Empty = 0xffff;
	Uint16 m_background;
	Uint16 m_playground;
	Uint16 m_foreground;
	Uint16 m_padding;
};

struct DiskParallaxLayer
{
	Uint16 m_texture;
	float m_scrollScale;
};

class FileException : public runtime_error
{
public:
	FileException(const string& path, const string& what)
		: runtime_error("FileException: '" + path + "' - " + what)
	{
	}
};

class FileHandle
{
public:
	FileHandle(const string& path, const char* mode)
	{
		m_rwops = SDL_RWFromFile(path.c_str(), mode);

		if (!m_rwops)
		{
			throw FileException(path, string("failed to open [") + mode + "]");
		}
	}

	~FileHandle()
	{
		SDL_RWclose(m_rwops);
	}

	operator SDL_RWops* ()
	{
		return m_rwops;
	}

	SDL_RWops* operator->()
	{
		return m_rwops;
	}

private:
	SDL_RWops* m_rwops = nullptr;
};

class BadMapException : public runtime_error
{
public:
	BadMapException(const string& path, const string& what)
		: runtime_error("Could not load map file '" + path + "' - " + what)
	{
	}
};

GameMap::GameMap(int numTilesX, int numTilesY)
	: m_numTilesX(numTilesX)
	, m_numTilesY(numTilesY)
{
	int numTiles = m_numTilesX * m_numTilesY;

	for (int i = 0; i < 3; ++i)
	{
		m_tileMaps[i].resize(numTiles, -1);
	}
}

void GameMap::Load(const string& path, const System& system)
{
	m_textures.clear();

	FileHandle file(path, "rb");

	DiskHeader dh;

	if (!SDL_RWread(file, &dh, sizeof(DiskHeader), 1))
	{
		throw BadMapException(path, "invalid format");
	}

	if (dh.m_magic != MAPMAGIC)
	{
		throw BadMapException(path, "bad header magic");
	}

	// Load tile maps
	m_numTilesX = dh.m_width;
	m_numTilesY = dh.m_height;

	int numTiles = m_numTilesX * m_numTilesY;
	vector<DiskTile> tileData;
	tileData.resize(numTiles);

	if (SDL_RWread(file, &tileData[0], sizeof(DiskTile), numTiles) != numTiles)
	{
		throw BadMapException(path, "unexpected end of file loading tile maps");
	}

	for (int i = 0; i < 3; ++i)
	{
		m_tileMaps[i].resize(numTiles);
	}

	for (int y = 0; y < m_numTilesY; ++y)
	{
		for (int x = 0; x < m_numTilesX; ++x)
		{
			int tileIndex = TileIndex(x, y);
			m_tileMaps[kBackground].at(tileIndex) = (tileData[tileIndex].m_background == DiskTile::Empty) ? -1 :
			                                        tileData[tileIndex].m_background;
			m_tileMaps[kPlayground].at(tileIndex) = (tileData[tileIndex].m_playground == DiskTile::Empty) ? -1 :
			                                        tileData[tileIndex].m_playground;
			m_tileMaps[kForeground].at(tileIndex) = (tileData[tileIndex].m_foreground == DiskTile::Empty) ? -1 :
			                                        tileData[tileIndex].m_foreground;
		}
	}

	// Load parallax layer data
	m_parallaxLayers.resize(dh.m_numParallaxLayers);
	m_parallaxScrollScales.resize(dh.m_numParallaxLayers);
	vector<DiskParallaxLayer> parallaxLayerData;
	parallaxLayerData.resize(dh.m_numParallaxLayers);

	if (SDL_RWread(file, &parallaxLayerData[0], sizeof(DiskParallaxLayer), dh.m_numParallaxLayers) != dh.m_numParallaxLayers)
	{
		throw BadMapException(path, "unexpected end of file loading parallax layer data");
	}

	for (int i = 0; i < dh.m_numParallaxLayers; ++i)
	{
		m_parallaxLayers[i] = parallaxLayerData[i].m_texture;
		m_parallaxScrollScales[i] = parallaxLayerData[i].m_scrollScale;
	}

	// Load textures
	string packedPaths;
	packedPaths.resize(dh.m_texturePathsSize);

	if (SDL_RWread(file, &packedPaths[0], 1, dh.m_texturePathsSize) != dh.m_texturePathsSize)
	{
		throw BadMapException(path, "unexpected end of file loading texture paths");
	}

	stringstream unpacker(packedPaths);
	string texturePath;

	while (getline(unpacker, texturePath, '?'))
	{
		m_textures.push_back(system.LoadTexture(texturePath));
	}
}

void GameMap::Save(const string& path)
{
	FileHandle file(path, "wb");

	DiskHeader dh;

	if (SDL_RWseek(file, sizeof(dh), RW_SEEK_SET) < 0)
	{
		throw FileException(path, "seek failed");
	}

	dh.m_magic = MAPMAGIC;
	dh.m_width = m_numTilesX;
	dh.m_height = m_numTilesY;
	dh.m_numParallaxLayers = m_parallaxLayers.size();

	// Write tiles
	int numTiles = m_numTilesX * m_numTilesY;
	vector<DiskTile> diskTiles;
	diskTiles.reserve(numTiles);

	for (int y = 0; y < m_numTilesY; ++y)
	{
		for (int x = 0; x < m_numTilesX; ++x)
		{
			DiskTile tile = { 0 };
			int tileIndex = TileIndex(x, y);
			tile.m_background = (m_tileMaps[kBackground].at(tileIndex) == -1) ? DiskTile::Empty : m_tileMaps[kBackground].at(tileIndex);
			tile.m_playground = (m_tileMaps[kPlayground].at(tileIndex) == -1) ? DiskTile::Empty : m_tileMaps[kPlayground].at(tileIndex);
			tile.m_foreground = (m_tileMaps[kForeground].at(tileIndex) == -1) ? DiskTile::Empty : m_tileMaps[kForeground].at(tileIndex);
			diskTiles.push_back(tile);
		}
	}

	if (SDL_RWwrite(file, &diskTiles[0], sizeof(DiskTile), numTiles) != numTiles)
	{
		throw FileException(path, "failed to write tile data");
	}


	// Write parallax data
	vector<DiskParallaxLayer> diskParallaxData;
	int numParallaxLayers = m_parallaxLayers.size();
	diskParallaxData.reserve(numParallaxLayers);

	for (int i = 0; i < numParallaxLayers; ++i)
	{
		DiskParallaxLayer parallaxData = { m_parallaxLayers[i], m_parallaxScrollScales[i] };
		diskParallaxData.push_back(parallaxData);
	}

	if (SDL_RWwrite(file, &diskParallaxData[0], sizeof(DiskParallaxLayer), numParallaxLayers) != numParallaxLayers)
	{
		throw FileException(path, "failed to write parallax data");
	}


	// Write texture paths
	vector<string> texturePaths;

	for (auto texture : m_textures)
	{
		string texturePath = texture.GetPath();

		if (texturePath.empty())
		{
			throw BadMapException(path, "cannot serialise reference to texture without its path");
		}

		texturePaths.push_back(texturePath);
	}

	ostringstream ss;
	copy(texturePaths.begin(), texturePaths.end(), ostream_iterator<string>(ss, "?"));
	dh.m_texturePathsSize = ss.str().length();

	if (SDL_RWwrite(file, &ss.str()[0], 1, dh.m_texturePathsSize) != dh.m_texturePathsSize)
	{
		throw FileException(path, "failed to write packed texture paths");
	}

	if (SDL_RWseek(file, 0, RW_SEEK_SET) < 0)
	{
		throw FileException(path, "seek failed");
	}

	if (SDL_RWwrite(file, &dh, sizeof(dh), 1) != 1)
	{
		throw FileException(path, "failed to write map header");
	}
}

int GameMap::GetTile(TileLayer layer, int x, int y)
{
	return m_tileMaps[layer].at(TileIndex(x, y));
}

void GameMap::SetTile(TileLayer layer, int x, int y, TexturePtr texture)
{
	m_tileMaps[layer].at(TileIndex(x, y)) = AddTexture(texture);
}

void GameMap::SetBackdrop(TexturePtr texture)
{
	SetParallaxLayer(0, 0.0f, texture);
}

void GameMap::SetParallaxLayer(int layer, float scrollScale, TexturePtr texture)
{
	if (m_parallaxLayers.size() <= layer)
	{
		m_parallaxLayers.resize(layer + 1, -1);
		m_parallaxScrollScales.resize(layer + 1, 0.0f);
	}

	m_parallaxLayers[layer] = AddTexture(texture);
	m_parallaxScrollScales[layer] = 0.0f;
}

void GameMap::Draw(SDL_Renderer* renderer, int scrollX, int scrollY)
{
	SDL_Rect screenRect = { 0, 0, 1280, 720 };

	if (m_parallaxLayers.size() > 0 && m_parallaxLayers[0] != -1)
	{
		SDL_RenderCopy(renderer, m_textures[m_parallaxLayers[0]], nullptr, &screenRect);
	}

	for (int i = 0; i < 3; ++i)
	{
		for (int y = 0; y < SCREENTILESY; ++y)
		{
			for (int x = 0; x < SCREENTILESX; ++x)
			{
				SDL_Rect tileRect = { x * 64, y * 64, 64, 64 };
				int tile = m_tileMaps[i].at(TileIndex(x + (scrollX >> 6), y + (scrollY >> 6)));
				TexturePtr tileTexture = (tile < 0) ? nullptr : m_textures[tile];

				if (tileTexture)
				{
					SDL_RenderCopy(renderer, tileTexture, nullptr, &tileRect);
				}
			}
		}

	}
}

int GameMap::TileIndex(int x, int y)
{
	return y * m_numTilesX + x;
}

int GameMap::AddTexture(TexturePtr texture)
{
	int textureIndex = -1;

	if (texture != nullptr)
	{
		textureIndex = find(m_textures.begin(), m_textures.end(), texture) - m_textures.begin();

		if (textureIndex >= m_textures.size())
		{
			m_textures.push_back(texture);
			textureIndex = m_textures.size() - 1;
		}
	}

	return textureIndex;
}
