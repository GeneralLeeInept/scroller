#include "System.h"

#include <SDL.h>
#include <SDL_image.h>

System::System(const string& title)
	: m_window(nullptr)
	, m_renderer(nullptr)
{
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
	{
		throw std::exception(SDL_GetError());
	}

	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		throw std::exception(IMG_GetError());
	}

	m_window = SDL_CreateWindow(title.c_str(), 100, 100, 1280, 720, 0);

	if (!m_window)
	{
		throw std::exception(SDL_GetError());
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!m_renderer)
	{
		throw std::exception(SDL_GetError());
	}
}

System::~System()
{
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	IMG_Quit();
	SDL_Quit();
}

SDL_Renderer* System::GetRenderer() const
{
	return m_renderer;
}