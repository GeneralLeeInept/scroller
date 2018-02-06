#pragma once

class IGameState;
class Input;
class System;

class GameController
{
public:
    enum GameState
    {
        kNone,
        kMainMenu,
        kNewGame,
        kMapEditor,
        kSpriteEditor
    };

    GameController(System& system, Input& input);

    void Run();
    void Quit();

    void GotoState(GameState nextState);

private:
    void DoStateChange();

    System& m_system;
    Input& m_input;
    shared_ptr<IGameState> m_currentState;
    GameState m_nextState = kMainMenu;
    bool m_quit = false;
};
