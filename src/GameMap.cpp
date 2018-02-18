#include "stdafx.h"

#include "FileHandle.h"
#include "GameMap.h"
#include "System.h"

#define MAPMAGIC FOURCC("GMAP")
#define SCREENTILESX (1280 / 64)
#define SCREENTILESY ((720 + 63) / 64)

struct MapHeader
{
    Uint32 m_magic;  // 'G','M','A','P'
    Uint16 m_width;  // In tiles
    Uint16 m_height; // In tiles
    Uint16 m_numParallaxLayers;
    Uint16 m_texturePathsSize;
};

struct TileHeader
{
    static const Uint16 Empty = 0xffff;
    Uint16 m_background;
    Uint16 m_playground;
    Uint16 m_foreground;
    Uint16 m_padding;
};

struct ParallaxLayer
{
    Uint16 m_texture;
    float m_scrollScale;
};

GameMap::GameMap(int numTilesX, int numTilesY)
    : m_numTilesX(numTilesX)
    , m_numTilesY(numTilesY)
{
    int numTiles = m_numTilesX * m_numTilesY;

    for (int i = 0; i < 3; ++i)
    {
        m_tileMaps[i].resize(numTiles, kEmptyTile);
    }
}

void GameMap::Load(const string& path, const System& system)
{
    m_textures.clear();

    FileHandle file(path, "rb");

    MapHeader dh;

    if (!SDL_RWread(file, &dh, sizeof(MapHeader), 1))
    {
        throw BadDataException(path, "invalid format");
    }

    if (dh.m_magic != MAPMAGIC)
    {
        throw BadDataException(path, "bad header magic");
    }

    // Load tile maps
    m_numTilesX = dh.m_width;
    m_numTilesY = dh.m_height;

    int numTiles = m_numTilesX * m_numTilesY;

    for (int i = 0; i < 3; ++i)
    {
        m_tileMaps[i].resize(numTiles);
    }

    for (auto layer : { kBackground, kPlayground, kForeground })
    {
        Uint16 dataSize;

        if (SDL_RWread(file, &dataSize, sizeof(Uint16), 1) != 1)
        {
            throw BadDataException(path, "unexpected end of file loading tile maps");
        }

        vector<Uint16> rleTiles;
        rleTiles.resize(dataSize);

        if (SDL_RWread(file, &rleTiles[0], sizeof(Uint16), dataSize) != dataSize)
        {
            throw BadDataException(path, "unexpected end of file loading tile maps");
        }

        int dataPtr = 0;

        for (int tile = 0; tile < numTiles;)
        {
            Uint16 datum = rleTiles[dataPtr++];
            Uint16 count = 1;

            if ((datum & 0x8000) == 0x8000)
            {
                count = datum & 0x7fff;
                datum = rleTiles[dataPtr++];
            }

            auto fillBegin = m_tileMaps[layer].begin() + tile;
            auto fillEnd = fillBegin + count;
            fill(fillBegin, fillEnd, datum);
            tile += count;
        }
    }

    // Load parallax layer data
    m_parallaxLayers.resize(dh.m_numParallaxLayers);
    m_parallaxScrollScales.resize(dh.m_numParallaxLayers);
    vector<ParallaxLayer> parallaxLayerData;
    parallaxLayerData.resize(dh.m_numParallaxLayers);

    if (SDL_RWread(file, &parallaxLayerData[0], sizeof(ParallaxLayer), dh.m_numParallaxLayers) != dh.m_numParallaxLayers)
    {
        throw BadDataException(path, "unexpected end of file loading parallax layer data");
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
        throw BadDataException(path, "unexpected end of file loading texture paths");
    }

    stringstream unpacker(packedPaths);
    string texturePath;

    while (getline(unpacker, texturePath, '?'))
    {
        m_textures.push_back(system.LoadTexture(texturePath));
    }
}

void GameMap::Save(const string& path) const
{
    FileHandle file(path, "wb");

    MapHeader dh;

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

    for (auto layer : { kBackground, kPlayground, kForeground })
    {
        vector<Uint16> tiles;
        tiles.reserve(numTiles);

        for (int tile = 0; tile < numTiles;)
        {
            // RLE
            Uint16 next = m_tileMaps[layer].at(tile);
            Uint16 count = 1;

            for (++tile; (tile < numTiles) && (m_tileMaps[layer].at(tile) == next) && (count < 0x7fff); ++tile, ++count)
                ;

            if (count > 1)
            {
                tiles.push_back(0x8000 | count);
                tiles.push_back(next);
            }
            else
            {
                tiles.push_back(next);
            }
        }

        Uint16 dataSize = static_cast<Uint16>(tiles.size());

        if (SDL_RWwrite(file, &dataSize, sizeof(Uint16), 1) != 1 || SDL_RWwrite(file, &tiles[0], sizeof(Uint16), dataSize) != dataSize)
        {
            throw FileException(path, "failed to write tile data");
        }
    }

    // Write parallax data
    vector<ParallaxLayer> diskParallaxData;
    int numParallaxLayers = m_parallaxLayers.size();
    diskParallaxData.reserve(numParallaxLayers);

    for (int i = 0; i < numParallaxLayers; ++i)
    {
        ParallaxLayer parallaxData = { m_parallaxLayers[i], m_parallaxScrollScales[i] };
        diskParallaxData.push_back(parallaxData);
    }

    if (SDL_RWwrite(file, &diskParallaxData[0], sizeof(ParallaxLayer), numParallaxLayers) != numParallaxLayers)
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
            throw BadDataException(path, "cannot serialise reference to texture without its path");
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

int GameMap::GetWidth() const
{
    return m_numTilesX;
}

int GameMap::GetHeight() const
{
    return m_numTilesY;
}

Uint16 GameMap::GetTile(TileLayer layer, int x, int y) const
{
    if (x < 0 || x > m_numTilesX || y < 0 || y > m_numTilesY)
    {
        return kEmptyTile;
    }

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

void GameMap::SetParallaxLayer(Uint16 layer, float scrollScale, TexturePtr texture)
{
    if (m_parallaxLayers.size() <= layer)
    {
        m_parallaxLayers.resize(layer + 1, -1);
        m_parallaxScrollScales.resize(layer + 1, 0.0f);
    }

    m_parallaxLayers[layer] = AddTexture(texture);
    m_parallaxScrollScales[layer] = 0.0f;
}

#define TILESIZE 64
#define SCREENTILESX (1280 / 64)
#define SCREENTILESY ((720 + 63) / 64)
#define MAPWIDTH (SCREENTILESX * 8)
#define MAPHEIGHT (SCREENTILESY * 8)
#define OFFSCREENTILES 16

static const SDL_Rect s_offscreenRects[4] = { { 0, 0, OFFSCREENTILES, MAPHEIGHT },
                                              { MAPWIDTH - OFFSCREENTILES, 0, OFFSCREENTILES, MAPHEIGHT },
                                              { OFFSCREENTILES, 0, MAPWIDTH - 2 * OFFSCREENTILES, OFFSCREENTILES },
                                              { OFFSCREENTILES, MAPHEIGHT - OFFSCREENTILES, MAPWIDTH - 2 * OFFSCREENTILES, OFFSCREENTILES } };

void GameMap::Draw(SDL_Renderer* renderer, int scrollX, int scrollY) const
{
    Draw(renderer, scrollX, scrollY, 0, 0, 1280, 720);
}

void GameMap::Draw(SDL_Renderer* renderer, int scrollX, int scrollY, int viewRectX, int viewRectY, int viewRectW, int viewRectH) const
{
    SDL_Rect screenRect = { viewRectX, viewRectY, viewRectW, viewRectH };

    if (m_parallaxLayers.size() > 0 && m_parallaxLayers[0] != 0)
    {
        SDL_RenderCopy(renderer, GetTexture(m_parallaxLayers[0]), nullptr, &screenRect);
    }

    int minScreenX = max(scrollX, 16 * 64);
    int maxScreenX = minScreenX + 1280;

    if (maxScreenX > (m_numTilesX - 16) * 64)
    {
        maxScreenX = (m_numTilesX - 16) * 64;
        minScreenX = maxScreenX - 1280;
    }

    int minScreenY = max(scrollY, 16 * 64);
    int maxScreenY = scrollY + 720;

    if (maxScreenY > (m_numTilesY - 16) * 64)
    {
        maxScreenY = (m_numTilesY - 16) * 64;
        minScreenY = maxScreenY - 720;
    }

    int minTileX = minScreenX >> 6;
    int maxTileX = (maxScreenX + 63) >> 6;
    int minTileY = (minScreenY) >> 6;
    int maxTileY = (maxScreenY + 63) >> 6;

    DrawLayer(renderer, kBackground, minTileX, minTileY, maxTileX, maxTileY, scrollX, scrollY);
    DrawLayer(renderer, kPlayground, minTileX, minTileY, maxTileX, maxTileY, scrollX, scrollY);
    DrawSprites(renderer, scrollX, scrollY);
    DrawLayer(renderer, kForeground, minTileX, minTileY, maxTileX, maxTileY, scrollX, scrollY);

    // Draw overlays
    SDL_SetRenderDrawColor(renderer, 128, 0, 0, 128);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    for (int i = 0; i < 4; ++i)
    {
        SDL_Rect drawRect = s_offscreenRects[i];
        drawRect.x = (drawRect.x - scrollX) * TILESIZE;
        drawRect.y = (drawRect.y - scrollY) * TILESIZE;
        drawRect.w *= TILESIZE;
        drawRect.h *= TILESIZE;
        SDL_RenderFillRect(renderer, &drawRect);
    }
}

int GameMap::TileIndex(int x, int y) const
{
    return y * m_numTilesX + x;
}

Uint16 GameMap::AddTexture(TexturePtr texture)
{
    Uint16 textureIndex = 0;

    if (texture != nullptr)
    {
        textureIndex = find(m_textures.begin(), m_textures.end(), texture) - m_textures.begin() + 1;

        if (textureIndex > m_textures.size())
        {
            m_textures.push_back(texture);
            textureIndex = m_textures.size();
        }
    }

    return textureIndex;
}

TexturePtr GameMap::GetTexture(Uint16 id) const
{
    if (id)
    {
        return m_textures.at(id - 1);
    }

    return nullptr;
}

void GameMap::DrawLayer(SDL_Renderer* renderer, TileLayer layer, int minTileX, int minTileY, int maxTileX, int maxTileY, int scrollX,
                        int scrollY) const
{
    for (int y = minTileY; y < maxTileY; ++y)
    {
        for (int x = minTileX; x < maxTileX; ++x)
        {
            Uint16 tile = m_tileMaps[layer].at(TileIndex(x, y));

            if (tile)
            {
                TexturePtr tileTexture = GetTexture(tile);
                SDL_Rect tileRect = { (x << 6) - scrollX, (y << 6) - scrollY, 64, 64 };
                SDL_RenderCopy(renderer, tileTexture, nullptr, &tileRect);
            }
        }
    }
}

void GameMap::DrawSprites(SDL_Renderer* renderer, int scrollX, int scrollY) const {}
