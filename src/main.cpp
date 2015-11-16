#include "stdafx.h"

#include "System.h"

int main(int argc, char** argv)
{
	try
	{
		System system("Super Ryan Land");

		bool quit = false;

		TexturePtr texture = system.LoadTexture("sprites/superryan.png");

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
	}
	catch (exception& e)
	{
		cout << "Error: " << e.what() << endl;
		return 1;
	}

	return 0;
}
