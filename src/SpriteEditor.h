#pragma once

#include "IGameState.h"

class GameController;
class Input;
class Sprite;
class System;

class SpriteEditor : public IGameState
{
public:
    SpriteEditor(const System& system, GameController& gameController, Input& input);

    void Update(Uint32 ms);
    void Draw(SDL_Renderer* renderer);

private:
    void MakeTestSprite(const System& system);
    void update_input();

    const System& m_system;
    GameController& m_gameController;
    Input& m_input;
    unique_ptr<Sprite> m_sprite;
    bool m_left = false;
    bool m_right = false;
};
