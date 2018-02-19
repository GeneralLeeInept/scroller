#pragma once

#include "IGameState.h"

#include "GameMap.h"
#include "Texture.h"

class GameController;
class Input;
class System;

class MapEditor : public IGameState
{
public:
    MapEditor(const System& system, GameController& gameController, Input& input);

    void Update(Uint32 ms);
    void Draw(SDL_Renderer* renderer);

private:
    void LoadResources(const System& system);
    void check_input();

    void draw_map(SDL_Renderer* renderer, int x, int y, int w, int h, float cx, float cy, float zoom);
    void draw_tile_palette(SDL_Renderer* renderer, int x, int y, int w, int h, float cx, float cy, float zoom);

    const System& m_system;
    Input& m_input;
    GameController& m_gameController;
    GameMap m_mapData;
    vector<TexturePtr> _tile_textures;
    vector<TexturePtr>::iterator m_tileBrush;
    vector<TexturePtr> m_backdropTextures;
    vector<TexturePtr>::iterator m_backdrop;
    int m_scrollX = 0;
    int m_scrollY = 0;
    TexturePtr m_cursor = nullptr;
    int m_cursorX = 0;
    int m_cursorY = 0;
    bool m_paint = false;
    bool m_erase = false;
    GameMap::TileLayer m_activeLayer = GameMap::kPlayground;
    int m_brushHintShowTime = 2000;


    typedef vector<uint32_t> MapRow;
    typedef vector<MapRow> MapLayer;
    typedef vector<MapLayer> Map;

    Map _map_data;
};
