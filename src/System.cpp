#include "System.h"

#include <SDL.h>

System::System(const string& title)
	: m_window(nullptr)
	, m_renderer(nullptr)
{
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
	{
		throw std::exception(SDL_GetError());
	}

	m_window = SDL_CreateWindow(title.c_str(), 100, 100, 640, 480, 0);

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
	SDL_DestroyWindow(m_window);

	SDL_Quit();
}
