#pragma once

#include "IGameState.h"

class GameController;
class System;

class GamePlay : public IGameState
{
public:
	GamePlay(const System& system, GameController& gameController);

	bool HandleEvent(SDL_Event& event);
	void Update(Uint32 ms);
	void Draw(SDL_Renderer* renderer);
};