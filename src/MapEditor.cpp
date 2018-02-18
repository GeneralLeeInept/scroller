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
    m_tileBrush = m_tileTextures.begin();
    m_backdrop = m_backdropTextures.begin();
    m_mapData.Load("maps/test.map", m_system);
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
    SDL_SetRenderDrawColor(renderer, 128, 0, 0, SDL_ALPHA_OPAQUE);

    // Draw map
    SDL_Rect mapEdit = { 16, 16, 900, 427 };
    SDL_RenderFillRect(renderer, &mapEdit);
    m_mapData.Draw(renderer, m_scrollX * TILESIZE, m_scrollY * TILESIZE, 16, 16, 900, 427);

    SDL_Rect miniMap = { 16, 475, 900, 229 };
    SDL_RenderFillRect(renderer, &miniMap);

    SDL_Rect tools = { 948, 16, 316, 688 };
    SDL_RenderFillRect(renderer, &tools);
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
            m_tileTextures.push_back(system.LoadTexture(path));
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
        if (m_tileBrush == m_tileTextures.begin())
        {
            m_tileBrush = m_tileTextures.end();
        }

        m_tileBrush = prev(m_tileBrush);

        m_brushHintShowTime = 2000;
    }
    else if (m_input.KeyReleased(SDL_SCANCODE_RIGHTBRACKET))
    {
        m_tileBrush = next(m_tileBrush);

        if (m_tileBrush == m_tileTextures.end())
        {
            m_tileBrush = m_tileTextures.begin();
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

void MapEditor::draw_map(int x, int y, int w, int h, float cx, float cy, float zoom)
{
    SDL_Rect clip_rect = { x, y, w, h };
    SDL_RenderSetClipRect(m_system.GetRenderer(), &clip_rect);

    for (MapLayer& layer : _map_data)
    {
        int tile_x = 0;
        int tile_y = 0;

        for (MapRow& row : layer)
        {
        }
    }

    SDL_RenderSetClipRect(m_system.GetRenderer(), nullptr);
}
