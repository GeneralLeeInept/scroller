#pragma once

#include "IGameState.h"

class GameController;
class Input;
class System;

class GamePlay : public IGameState
{
public:
	GamePlay(const System& system, GameController& gameController, Input& input);

	void Update(Uint32 ms);
	void Draw(SDL_Renderer* renderer);

private:
	void UpdateWorld(Uint32 ms);
	void UpdatePlayer(Uint32 ms);
	void MoveCamera(Uint32 ms);
	void UpdateEntities(Uint32 ms);

	System& m_system;
	GameController& m_gameController;
	Input& m_input;
};