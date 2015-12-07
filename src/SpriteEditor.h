#pragma once

#include "IGameState.h"
#include "Texture.h"

class GameController;
class System;

class SpriteEditor : public IGameState
{
public:
	SpriteEditor(const System& system, GameController& gameController);

	bool HandleEvent(SDL_Event& event);
	void Update(Uint32 ms);
	void Draw(SDL_Renderer* renderer);

private:
	const System& m_system;
	GameController& m_gameController;
	TexturePtr m_spriteSheet;
	Uint32 m_frameTimer = 0;
	int m_frame = 0;
};
