#include "stdafx.h"

#include "MainMenu.h"
#include "System.h"

#define NUMITEMS 4
#define ITEMPOSX 640
#define FIRSTITEMY 335
#define ITEMSPACINGY 100
#define SCALEFREQ 500

static const char* Images[NUMITEMS * 2] =
{
	"menu/newgamelo.png", "menu/newgamehi.png",
	"menu/mapeditorlo.png", "menu/mapeditorhi.png",
	"menu/spriteeditorlo.png", "menu/spriteeditorhi.png",
	"menu/quitlo.png", "menu/quithi.png"
};

static SDL_Point Dims[NUMITEMS] =
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

	for (int i = 0; i < NUMITEMS; ++i)
	{
		MenuItem mi;
		mi.m_images[0] = system.LoadTexture(Images[i * 2]);
		mi.m_images[1] = system.LoadTexture(Images[(i * 2) + 1]);
		mi.m_dims = Dims[i];
		mi.m_position.x = ITEMPOSX;
		mi.m_position.y = FIRSTITEMY + (ITEMSPACINGY * i) + (mi.m_dims.y >> 1);
		mi.m_state = 0;
		mi.m_stateTime = 0;
		m_menu.push_back(mi);
	}

	m_menu[0].m_state = 1;
}

bool MainMenu::HandleEvent(SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN)
	{
		SDL_KeyboardEvent& ke = reinterpret_cast<SDL_KeyboardEvent&>(event);
		switch (ke.keysym.scancode)
		{
			case SDL_SCANCODE_UP:
			{
				m_nextActive = (m_activeItem == 0) ? -1 : m_activeItem - 1;
				return true;
			}
			case SDL_SCANCODE_DOWN:
			{
				m_nextActive = (m_activeItem == NUMITEMS - 1) ? -1 : m_activeItem + 1;
				return true;
			}
			case SDL_SCANCODE_RETURN:
			{
				return true;
			}
		}
	}
	return false;
}

void MainMenu::Update(Uint32 ms)
{
	for (int i = 0; i < NUMITEMS; ++i)
	{
		MenuItem& mi = m_menu.at(i);
		mi.m_stateTime += ms;
	}

	if (m_nextActive != -1)
	{
		m_menu[m_activeItem].m_state = 0;
		m_menu[m_activeItem].m_stateTime = 0;
		m_menu[m_nextActive].m_state = 1;
		m_menu[m_nextActive].m_stateTime = 0;
		m_activeItem = m_nextActive;
		m_nextActive = -1;
	}
}

void MainMenu::Draw(SDL_Renderer* renderer)
{
	SDL_RenderCopy(renderer, m_backdropImage, nullptr, nullptr);

	SDL_Rect sourceRect = { 0, 0, 703, 225 };
	SDL_Rect destRect = { (1280 - 703) >> 1, 52, 703, 225 };
	SDL_RenderCopy(renderer, m_titleImage, &sourceRect, &destRect);

	for (int i = 0; i < NUMITEMS; ++i)
	{
		MenuItem& mi = m_menu.at(i);
		SDL_Rect sourceRect = { 0, 0, mi.m_dims.x, mi.m_dims.y };
		int destWidth = mi.m_dims.x;
		int destHeight = mi.m_dims.y;
		
		if (mi.m_state == 1)
		{
			float t = (3.141f * mi.m_stateTime) / SCALEFREQ;
			float scale = 1.0f + (0.5f + sinf(t) * 0.5f) * 0.25f;
			destWidth = static_cast<int>(mi.m_dims.x * scale);
			destHeight = static_cast<int>(mi.m_dims.y * scale);
		}

		SDL_Rect destRect = { mi.m_position.x - (destWidth >> 1), mi.m_position.y - (destHeight >> 1), destWidth, destHeight };
		SDL_RenderCopy(renderer, mi.m_images[mi.m_state], &sourceRect, &destRect);
	}
}
