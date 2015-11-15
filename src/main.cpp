#include <iostream>

#include "System.h"

#include <SDL.h>
#include <SDL_image.h>

int main(int argc, char** argv)
{
	SDL_Surface* image = nullptr;
	SDL_Texture* texture = nullptr;

	try
	{
		System system("Super Ryan Land");

		image = IMG_Load("sprites/superryan.png");

		if (!image)
		{
			throw std::exception("Failed to load sprite sheet");
		}

		texture = SDL_CreateTextureFromSurface(system.GetRenderer(), image);

		if (!texture)
		{
			throw std::exception("Failed to create sprite texture");
		}

		bool quit = false;

		SDL_Rect destRect;
		destRect.w = 256;
		destRect.h = destRect.w << 1;
		destRect.x = (1280 - destRect.w) >> 1;
		destRect.y = (720 - destRect.h) >> 1;

		int frame = 0;
		Uint32 ticks = SDL_GetTicks();

		while (!quit)
		{
			//Handle events on queue
			SDL_Event e;

			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}

			// Update state
			Uint32 nextTicks = SDL_GetTicks();

			if (nextTicks - ticks > 17)
			{
				if (++frame >= 22)
				{
					frame = 0;
				}

				ticks = nextTicks;
			}

			//Clear screen
			SDL_SetRenderDrawColor(system.GetRenderer(), 84, 200, 255, 255);
			SDL_RenderClear(system.GetRenderer());
			SDL_SetRenderDrawColor(system.GetRenderer(), 0, 0, 0, 255);

			//Render texture to screen
			SDL_Rect srcRect;
			srcRect.x = (frame & 0xf) << 7;
			srcRect.y = (frame >> 4) << 8;
			srcRect.w = 128;
			srcRect.h = 256;
			SDL_RenderCopy(system.GetRenderer(), texture, &srcRect, &destRect);

			//Update screen
			SDL_RenderPresent(system.GetRenderer());
		}

		SDL_DestroyTexture(texture);
		SDL_FreeSurface(image);
	}
	catch (std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
