#include "stdafx.h"

#include "CollisionTest.h"
#include "MapEditor.h"
#include "System.h"

int main(int argc, char** argv)
{
	try
	{
		System system("Super Ryan Land");
		SDL_ShowCursor(0);

		bool quit = false;

		//MapEditor editor(system);
		CollisionTest collision(system);
		Uint32 lastTicks = SDL_GetTicks();

		collision.Update(0);
		collision.Draw(system.GetRenderer());
		SDL_RenderPresent(system.GetRenderer());

		while (!quit)
		{
			//Handle events on queue
			SDL_Event e;

			while (SDL_PollEvent(&e) != 0)
			{
				if (!collision.HandleEvent(e))
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
				}
			}

			// Update state
			Uint32 ticks = SDL_GetTicks();
			collision.Update(ticks - lastTicks);
			lastTicks = ticks;

			//Draw
			collision.Draw(system.GetRenderer());

			//Update screen
			SDL_RenderPresent(system.GetRenderer());
		}
	}
	catch (exception& e)
	{
		cout << "Error: " << e.what() << endl;
		return 1;
	}

	return 0;
}
