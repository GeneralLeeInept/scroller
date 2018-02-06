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
    // Draw map
    m_mapData.Draw(renderer, m_scrollX * TILESIZE, m_scrollY * TILESIZE);

    // Draw overlays
    SDL_SetRenderDrawColor(renderer, 128, 0, 0, 128);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    for (int i = 0; i < 4; ++i)
    {
        SDL_Rect drawRect = s_offscreenRects[i];
        drawRect.x = (drawRect.x - m_scrollX) * TILESIZE;
        drawRect.y = (drawRect.y - m_scrollY) * TILESIZE;
        drawRect.w *= TILESIZE;
        drawRect.h *= TILESIZE;
        SDL_RenderFillRect(renderer, &drawRect);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // Draw cursor
    SDL_Rect cursorRect = { m_cursorX * TILESIZE, m_cursorY * TILESIZE, TILESIZE, TILESIZE };

    SDL_RenderCopy(renderer, m_cursor, nullptr, &cursorRect);

    // Draw status
    if (m_brushHintShowTime != 0)
    {
        int alpha = 255;

        if (m_brushHintShowTime < 1000)
        {
            alpha = (m_brushHintShowTime * 255) / 1000;
        }
        else if (m_brushHintShowTime > 2000)
        {
            alpha = ((3000 - m_brushHintShowTime) * 255) / 1000;
        }

        SDL_SetTextureAlphaMod(*m_tileBrush, alpha);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_Rect brushRect = { 16, 16, 64, 64 };
        SDL_RenderCopy(renderer, *m_tileBrush, nullptr, &brushRect);
        SDL_SetTextureAlphaMod(*m_tileBrush, 255);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
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
