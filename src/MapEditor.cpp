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

MapEditor::MapEditor(const System& system, GameController& gameController, Input& input)
    : m_system(system)
    , m_gameController(gameController)
    , m_input(input)
{
    LoadResources(system);

    _map_data =
    {{
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        {  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0 },
        {  0,  0,  0, 25, 22,  0,  0,  0,  0,  0,  0, 22,  0,  0,  0,  0,  0,  0, 25, 22, 25,  0,  0,  0,  0,  0, 25, 22,  0,  0,  0,  0,  0,  0, 22,  0,  0,  0,  0,  0,  0, 25, 22, 25,  0,  0 },
        {  0,  0, 25, 22, 22,  0,  0,  0,  0,  0,  0, 22,  0,  0,  0,  0,  0, 25, 22, 22, 22, 25,  0,  0,  0, 25, 22, 22,  0,  0,  0,  0,  0,  0, 22,  0,  0,  0,  0,  0, 25, 22, 22, 22, 25,  0 },
        { 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25 },
        { 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22 },
        { 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22 },
        { 22, 22, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22 },
        { 22, 21,  0,  0,  0, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22 },
        { 22, 21,  0,  0,  0, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22 },
        { 22, 21,  0,  0,  0, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22 },
        { 22, 21,  0,  0,  0, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22 },
        { 22, 22, 25, 25, 25, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22 },
        { 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22 },
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
}

void MapEditor::Draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
 
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    // Draw map
    SDL_Rect mapEdit = { 16, 16, 900, 427 };
    SDL_RenderFillRect(renderer, &mapEdit);
    draw_map(renderer, 16, 16, 900, 427, 0.0f, 0.0f, 1.0f);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_Rect miniMap = { 16, 475, 900, 229 };
    SDL_RenderFillRect(renderer, &miniMap);
    draw_minimap(renderer, 16, 475, 900, 229, 0.0f, 0.0f, 1.0f);

    SDL_Rect tools = { 948, 16, 316, 688 };
    SDL_RenderFillRect(renderer, &tools);
    draw_tile_palette(renderer, 948, 16, 316, 688, 0.0f, 0.0f, 1.0f);
}

void MapEditor::LoadResources(const System& system)
{
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
                SDL_Rect tile_rect = { x + ((tile_x  - m_scrollX) * tile_size), y + ((tile_y - m_scrollY) * tile_size), tile_size, tile_size };

                if (tile)
                {
                    TexturePtr tileTexture = _tile_textures[tile - 1];
                    SDL_RenderCopy(renderer, tileTexture, nullptr, &tile_rect);
                }

                ++tile_x;
            }

            ++tile_y;
            tile_x = 0;
        }
    }

    SDL_RenderSetClipRect(m_system.GetRenderer(), nullptr);
}

void MapEditor::draw_minimap(SDL_Renderer* renderer, int x, int y, int w, int h, float cx, float cy, float zoom)
{
    static const int tile_size = 4;

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
                SDL_SetRenderDrawColor(renderer, tile ? 80 : 0, tile ? 80 : 0, tile ? 0 : 80, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &tile_rect);
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