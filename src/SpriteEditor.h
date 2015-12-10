#pragma once

#include "IGameState.h"

class GameController;
class Sprite;
class System;

class SpriteEditor : public IGameState
{
public:
	SpriteEditor(const System& system, GameController& gameController);

	bool HandleEvent(SDL_Event& event);
	void Update(Uint32 ms);
	void Draw(SDL_Renderer* renderer);

private:
	void MakeTestSprite(const System& system);

	const System& m_system;
	GameController& m_gameController;
	unique_ptr<Sprite> m_sprite;
	bool m_left = false;
	bool m_right = false;
};
