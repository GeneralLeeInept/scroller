#include "stdafx.h"

#include "GameController.h"
#include "SpriteEditor.h"
#include "System.h"

SpriteEditor::SpriteEditor(const System& system, GameController& gameController)
	: m_system(system)
	, m_gameController(gameController)
{
	m_spriteSheet = system.LoadTexture("sprites/superryan.png");
}

bool SpriteEditor::HandleEvent(SDL_Event& event)
{
	if (event.type = SDL_KEYUP)
	{
		SDL_KeyboardEvent& ke = reinterpret_cast<SDL_KeyboardEvent&>(event);

		if (ke.keysym.scancode == SDL_SCANCODE_X)
		{
			m_gameController.GotoState(GameController::kMainMenu);
			return true;
		}
	}

	return false;
}

void SpriteEditor::Update(Uint32 ms)
{
	// Update state
	m_frameTimer += ms;

	while (m_frameTimer > 33)
	{
		if (++m_frame >= 22)
		{
			m_frame = 0;
		}

		m_frameTimer -= 33;
	}
}

void SpriteEditor::Draw(SDL_Renderer* renderer)
{
	SDL_Rect destRect;
	destRect.w = 256;
	destRect.h = destRect.w << 1;
	destRect.x = (1280 - destRect.w) >> 1;
	destRect.y = (720 - destRect.h) >> 1;

	//Clear screen
	SDL_SetRenderDrawColor(renderer, 84, 200, 255, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	//Render texture to screen
	SDL_Rect srcRect;
	srcRect.x = (m_frame & 0xf) << 7;
	srcRect.y = (m_frame >> 4) << 8;
	srcRect.w = 128;
	srcRect.h = 256;
	SDL_RenderCopy(renderer, m_spriteSheet, &srcRect, &destRect);
}