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
	struct MenuItem
	{
		TexturePtr m_images[2];
		SDL_Point m_dims;
		SDL_Point m_position;
		int m_state;
		Uint32 m_stateTime;
	};

	GameController& m_gameController;
	TexturePtr m_backdropImage;
	TexturePtr m_titleImage;
	vector<MenuItem> m_menu;
	int m_activeItem = 0;
	int m_nextActive = -1;
};