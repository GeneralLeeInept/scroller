#include "stdafx.h"

#include "GameController.h"
#include "IGameState.h"
#include "System.h"

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

		if (m_nextState != nullptr)
		{
			m_currentState = m_nextState;
			m_nextState = nullptr;
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

void GameController::GotoState(shared_ptr<IGameState> nextState)
{
	m_nextState = nextState;
}