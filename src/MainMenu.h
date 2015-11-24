#pragma once

#include "IGameState.h"
#include "Texture.h"

class GameController;
class System;

class MainMenu : public IGameState
{
public:
	MainMenu(System& system, GameController& gameController);

	bool HandleEvent(SDL_Event& event);
	void Update(Uint32 ms);
	void Draw(SDL_Renderer* renderer);

private:
	GameController& m_gameController;

	TexturePtr m_backdropImage;
	TexturePtr m_titleImage;

	struct MenuItem
	{
		TexturePtr m_images[2];
		SDL_Point m_dims;
		SDL_Point m_position;
	};

	vector<MenuItem> m_menu;
};