#include "stdafx.h"

#include "GameController.h"
#include "IGameState.h"
#include "System.h"

#include "CollisionTest.h"
#include "MainMenu.h"
#include "MapEditor.h"

GameController::GameController(System& system)
	: m_system(system)
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

		//Handle events on queue
		SDL_Event e;

		while (SDL_PollEvent(&e) != 0)
		{
			if (!m_currentState || !m_currentState->HandleEvent(e))
			{
				if (e.type == SDL_QUIT)
				{
					Quit();
				}
			}
		}

		if (m_currentState)
		{
			// Update state
			m_currentState->Update(ticks - lastTicks);

			// Draw
			m_currentState->Draw(m_system.GetRenderer());
		}
		else
		{
			SDL_SetRenderDrawColor(m_system.GetRenderer(), 128, 0, 128, 255);
			SDL_RenderClear(m_system.GetRenderer());
		}

		// Update screen
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
			m_currentState.reset(new MainMenu(m_system, *this));
			break;
		}
		case kNewGame:
		{
			m_currentState.reset(new CollisionTest(m_system));
			break;
		}
		case kMapEditor:
		{
			m_currentState.reset(new MapEditor(m_system));
			break;
		}
	}

	m_nextState = kNone;
}