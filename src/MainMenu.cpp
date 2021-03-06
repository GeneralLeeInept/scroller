#include "stdafx.h"

#include "GameController.h"
#include "Input.h"
#include "MainMenu.h"
#include "System.h"

#define NUMITEMS 4
#define ITEMPOSX 640
#define FIRSTITEMY 335
#define ITEMSPACINGY 100
#define SCALEFREQ 500

static const char* Images[NUMITEMS * 2] = { "menu/newgamelo.png",      "menu/newgamehi.png",      "menu/mapeditorlo.png", "menu/mapeditorhi.png",
                                            "menu/spriteeditorlo.png", "menu/spriteeditorhi.png", "menu/quitlo.png",      "menu/quithi.png" };

static SDL_Point Dims[NUMITEMS] = { { 430, 63 }, { 498, 72 }, { 592, 72 }, { 182, 59 } };

enum MenuItems
{
    kNewGame,
    kMapEditor,
    kSpriteEditor,
    kQuit
};

enum ItemStates
{
    kInactive,
    kActive
};

MainMenu::MainMenu(System& system, GameController& gameController, Input& input)
    : m_gameController(gameController)
    , m_input(input)
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
        mi.m_state = kInactive;
        mi.m_stateTime = 0;
        m_menu.push_back(mi);
    }

    m_menu[0].m_state = kActive;
}

#if 0
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
				m_accept = true;
				return true;
			}
		}
	}

	return false;
}
#endif

void MainMenu::Update(Uint32 ms)
{
    for (int i = 0; i < NUMITEMS; ++i)
    {
        MenuItem& mi = m_menu.at(i);
        mi.m_stateTime += ms;
    }

    if (m_input.PreviousItem())
    {
        m_nextActive = (m_activeItem == 0) ? -1 : m_activeItem - 1;
    }
    else if (m_input.NextItem())
    {
        m_nextActive = (m_activeItem == NUMITEMS - 1) ? -1 : m_activeItem + 1;
    }

    if (m_nextActive != -1)
    {
        m_menu[m_activeItem].m_state = kInactive;
        m_menu[m_activeItem].m_stateTime = 0;
        m_menu[m_nextActive].m_state = kActive;
        m_menu[m_nextActive].m_stateTime = 0;
        m_activeItem = m_nextActive;
        m_nextActive = -1;
    }
    else if (m_input.SelectItem())
    {
        switch (m_activeItem)
        {
            case kNewGame:
            {
                m_gameController.GotoState(GameController::kNewGame);
                break;
            }

            case kMapEditor:
            {
                m_gameController.GotoState(GameController::kMapEditor);
                break;
            }

            case kSpriteEditor:
            {
                m_gameController.GotoState(GameController::kSpriteEditor);
                break;
            }

            case kQuit:
            {
                m_gameController.Quit();
                break;
            }
        }
    }

    m_accept = false;
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
