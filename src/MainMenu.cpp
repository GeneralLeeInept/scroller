#include "stdafx.h"

#include "MainMenu.h"
#include "System.h"

static const char* Images[] =
{
	"menu/newgamehi.png", "menu/newgamelo.png",
	"menu/mapeditorhi.png", "menu/mapeditorlo.png",
	"menu/spriteeditorhi.png", "menu/spriteeditorlo.png",
	"menu/quithi.png", "menu/quitlo.png"
};

static SDL_Point Dims[] =
{
	{ 430, 63 },
	{ 498, 72 },
	{ 592, 72 },
	{ 182, 59 }
};

MainMenu::MainMenu(System& system, GameController& gameController)
	: m_gameController(gameController)
{
	m_backdropImage = system.LoadTexture("backdrops/spooky.png");
	m_titleImage = system.LoadTexture("menu/title.png");

	for (int i = 0; i < 4; ++i)
	{
		MenuItem mi;
		mi.m_images[0] = system.LoadTexture(Images[i * 2]);
		mi.m_images[1] = system.LoadTexture(Images[(i * 2) + 1]);
		mi.m_dims = Dims[i];
		mi.m_position.x = (1280 - mi.m_dims.x) >> 1;
		mi.m_position.y = 335 + (100 * i);
		m_menu.push_back(mi);
	}
}

bool MainMenu::HandleEvent(SDL_Event& event)
{
	return false;
}

void MainMenu::Update(Uint32 ms)
{
}

void MainMenu::Draw(SDL_Renderer* renderer)
{
	SDL_RenderCopy(renderer, m_backdropImage, nullptr, nullptr);

	SDL_Rect sourceRect = { 0, 0, 703, 225 };
	SDL_Rect destRect = { (1280 - 703) >> 1, 52, 703, 225 };
	SDL_RenderCopy(renderer, m_titleImage, &sourceRect, &destRect);

	for (int i = 0; i < 4; ++i)
	{
		SDL_Rect sourceRect = { 0, 0, m_menu[i].m_dims.x, m_menu[i].m_dims.y };
		SDL_Rect destRect = { m_menu[i].m_position.x, m_menu[i].m_position.y, m_menu[i].m_dims.x, m_menu[i].m_dims.y };
		SDL_RenderCopy(renderer, i ? m_menu[i].m_images[1] : m_menu[i].m_images[0], &sourceRect, &destRect);
	}
}
