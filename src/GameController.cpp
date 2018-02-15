#include "stdafx.h"

#include "GameController.h"
#include "IGameState.h"
#include "System.h"

#include "CollisionTest.h"
#include "Input.h"
#include "MainMenu.h"
#include "MapEditor.h"
#include "SpriteEditor.h"

GameController::GameController(System& system, Input& input)
    : m_system(system)
    , m_input(input)
{
}

void GameController::Run()
{
    Uint32 lastTicks = SDL_GetTicks();

    while (!m_quit)
    {
        Uint32 ticks = SDL_GetTicks();

        if (m_nextState != kNone)
        {
            DoStateChange();
            lastTicks = ticks; // Do a 0 ms update first time
        }

        //Handle events
        SDL_Event e;

        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                Quit();
            }
        }

        // Update input
        m_input.Update();

        // Update state
        if (m_currentState)
        {
            m_currentState->Update(ticks - lastTicks);
            m_currentState->Draw(m_system.GetRenderer());
        }
        else
        {
            SDL_SetRenderDrawColor(m_system.GetRenderer(), 128, 0, 128, 255);
            SDL_RenderClear(m_system.GetRenderer());
        }

        // Flip
        SDL_RenderPresent(m_system.GetRenderer());

        lastTicks = ticks;
    }
}

void GameController::Quit()
{
    m_quit = true;
}

void GameController::GotoState(GameState nextState)
{
    m_nextState = nextState;
}

void GameController::DoStateChange()
{
    switch (m_nextState)
    {
        case kMainMenu:
        {
            m_currentState.reset(new MainMenu(m_system, *this, m_input));
            break;
        }

        case kNewGame:
        {
            m_currentState.reset(new CollisionTest(m_system, *this, m_input));
            break;
        }

        case kMapEditor:
        {
            m_currentState.reset(new MapEditor(m_system, *this, m_input));
            break;
        }

        case kSpriteEditor:
        {
            m_currentState.reset(new SpriteEditor(m_system, *this, m_input));
            break;
        }
    }

    m_nextState = kNone;
}
