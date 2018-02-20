#include "stdafx.h"

#include "GameController.h"
#include "Input.h"
#include "MapEditor.h"
#include "System.h"

#define TILESIZE 64
#define SCREENTILESX (1280 / 64)
#define SCREENTILESY ((720 + 63) / 64)
#define MAXCURSORX (SCREENTILESX - 1)
#define MAXCURSORY (SCREENTILESY - 1)
#define MAPWIDTH (SCREENTILESX * 8)
#define MAPHEIGHT (SCREENTILESY * 8)
#define MAXSCROLLX (MAPWIDTH - SCREENTILESX)
#define MAXSCROLLY (MAPHEIGHT - SCREENTILESY)
#define OFFSCREENTILES 16

static const SDL_Rect s_offscreenRects[4] = { { 0, 0, OFFSCREENTILES, MAPHEIGHT },
                                              { MAPWIDTH - OFFSCREENTILES, 0, OFFSCREENTILES, MAPHEIGHT },
                                              { OFFSCREENTILES, 0, MAPWIDTH - 2 * OFFSCREENTILES, OFFSCREENTILES },
                                              { OFFSCREENTILES, MAPHEIGHT - OFFSCREENTILES, MAPWIDTH - 2 * OFFSCREENTILES, OFFSCREENTILES } };

MapEditor::MapEditor(const System& system, GameController& gameController, Input& input)
    : m_system(system)
    , m_gameController(gameController)
    , m_input(input)
{
    LoadResources(system);
    m_tileBrush = _tile_textures.begin();
    m_backdrop = m_backdropTextures.begin();
//    m_mapData.Load("maps/test.map", m_system);

    _map_data =
    {{
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 1, 1, 1, 1 },
    }};
}

void MapEditor::Update(Uint32 ms)
{
    _CRT_UNUSED(ms);

    check_input();

    m_cursorX = max(0, min(m_cursorX, MAXCURSORX));
    m_cursorY = max(0, min(m_cursorY, MAXCURSORY));

    m_scrollX = max(0, min(m_scrollX, MAXSCROLLX));
    m_scrollY = max(0, min(m_scrollY, MAXSCROLLY));

    if (m_paint || m_erase)
    {
        m_mapData.SetTile(m_activeLayer, m_cursorX + m_scrollX, m_cursorY + m_scrollY, m_paint ? *m_tileBrush : nullptr);
    }

    if (m_brushHintShowTime != 0)
    {
        m_brushHintShowTime -= ms;

        if (m_brushHintShowTime < 0)
        {
            m_brushHintShowTime = 0;
        }
    }
}

void MapEditor::Draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    // Draw map
    SDL_Rect mapEdit = { 16, 16, 900, 427 };
    SDL_RenderFillRect(renderer, &mapEdit);
    draw_map(renderer, 16, 16, 900, 427, 0.0f, 0.0f, 1.0f);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_Rect miniMap = { 16, 475, 900, 229 };
    SDL_RenderFillRect(renderer, &miniMap);

    SDL_Rect tools = { 948, 16, 316, 688 };
    SDL_RenderFillRect(renderer, &tools);
    draw_tile_palette(renderer, 948, 16, 316, 688, 0.0f, 0.0f, 1.0f);
}

void MapEditor::LoadResources(const System& system)
{
    m_mapData.SetBackdrop(system.LoadTexture("backdrops/spooky.png"));
    m_cursor = system.LoadTexture("mapeditor/cursor.png");

    vector<string> tileTextures = system.GetFilesInFolder("tiles/");

    for (auto path : tileTextures)
    {
        try
        {
            _tile_textures.push_back(system.LoadTexture(path));
        }
        catch (exception& e)
        {
            _CRT_UNUSED(e);
        }
    }

    vector<string> backdropTextures = system.GetFilesInFolder("backdrops/");

    for (auto path : backdropTextures)
    {
        try
        {
            m_backdropTextures.push_back(system.LoadTexture(path));
        }
        catch (exception& e)
        {
            _CRT_UNUSED(e);
        }
    }
}

void MapEditor::check_input()
{
    m_input.GetMousePosition(&m_cursorX, &m_cursorY);
    m_cursorX /= TILESIZE;
    m_cursorY /= TILESIZE;

    m_paint = m_input.MouseButtonDown(SDL_BUTTON_LEFT);
    m_erase = m_input.MouseButtonDown(SDL_BUTTON_RIGHT);

    if (m_input.KeyReleased(SDL_SCANCODE_LEFTBRACKET))
    {
        if (m_tileBrush == _tile_textures.begin())
        {
            m_tileBrush = _tile_textures.end();
        }

        m_tileBrush = prev(m_tileBrush);

        m_brushHintShowTime = 2000;
    }
    else if (m_input.KeyReleased(SDL_SCANCODE_RIGHTBRACKET))
    {
        m_tileBrush = next(m_tileBrush);

        if (m_tileBrush == _tile_textures.end())
        {
            m_tileBrush = _tile_textures.begin();
        }

        m_brushHintShowTime = 2000;
    }

    if (m_input.KeyReleased(SDL_SCANCODE_F1))
    {
        m_activeLayer = GameMap::kBackground;
    }
    else if (m_input.KeyReleased(SDL_SCANCODE_F2))
    {
        m_activeLayer = GameMap::kPlayground;
    }
    else if (m_input.KeyReleased(SDL_SCANCODE_F3))
    {
        m_activeLayer = GameMap::kForeground;
    }

    if (m_input.KeyReleased(SDL_SCANCODE_S))
    {
        m_mapData.Save("maps/test.map");
    }
    else if (m_input.KeyReleased(SDL_SCANCODE_L))
    {
        m_mapData.Load("maps/test.map", m_system);
    }

    if (m_input.KeyReleased(SDL_SCANCODE_B))
    {
        m_backdrop = next(m_backdrop);

        if (m_backdrop == m_backdropTextures.end())
        {
            m_backdrop = m_backdropTextures.begin();
        }

        m_mapData.SetBackdrop(*m_backdrop);
    }

    //if (m_input.KeyReleased(SDL_SCANCODE_X))
    //{
    //    m_mapData.Save("maps/test.map");
    //    m_gameController.GotoState(GameController::kNewGame);
    //}

    if (m_input.KeyDown(SDL_SCANCODE_LEFT))
    {
        --m_scrollX;
    }
    else if (m_input.KeyDown(SDL_SCANCODE_RIGHT))
    {
        ++m_scrollX;
    }

    if (m_input.KeyDown(SDL_SCANCODE_UP))
    {
        --m_scrollY;
    }
    else if (m_input.KeyDown(SDL_SCANCODE_DOWN))
    {
        ++m_scrollY;
    }
}

void MapEditor::draw_map(SDL_Renderer* renderer, int x, int y, int w, int h, float cx, float cy, float zoom)
{
    static const int tile_size = 32;

    SDL_Rect clip_rect = { x, y, w, h };
    SDL_RenderSetClipRect(renderer, &clip_rect);

    for (MapLayer& layer : _map_data)
    {
        int tile_x = 0;
        int tile_y = 0;

        for (MapRow& row : layer)
        {
            for (uint32_t tile : row)
            {
                SDL_Rect tile_rect = { x + (tile_x * tile_size), y + (tile_y * tile_size), tile_size, tile_size };

                if (tile)
                {
                    TexturePtr tileTexture = _tile_textures[25];
                    SDL_RenderCopy(renderer, tileTexture, nullptr, &tile_rect);
                }

                ++tile_x;
            }

            ++tile_y;
            tile_x = 0;
        }
    }

    SDL_RenderSetClipRect(renderer, nullptr);
}

void MapEditor::draw_tile_palette(SDL_Renderer* renderer, int x, int y, int w, int h, float cx, float cy, float zoom)
{
    static const int tile_size = 64;

    SDL_Rect clip_rect = { x, y, w, h };
    SDL_RenderSetClipRect(renderer, &clip_rect);

    int tiles_per_row = w / (tile_size + 4);
    int tile_x = 0;
    int tile_y = 0;

    for (const TexturePtr& tile_texture : _tile_textures)
    {
        SDL_Rect tile_rect = { x + (tile_x * (tile_size + 4)) + 2, y + (tile_y * (tile_size + 4)) + 2, tile_size, tile_size };
        SDL_RenderCopy(renderer, tile_texture, nullptr, &tile_rect);

        if (++tile_x >= tiles_per_row)
        {
            tile_x = 0;
            tile_y++;
        }
    }

    SDL_RenderSetClipRect(renderer, nullptr);
}