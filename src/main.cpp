#include "stdafx.h"

#include "MapEditor.h"
#include "System.h"

int main(int argc, char** argv)
{
	try
	{
		System system("Super Ryan Land");
		SDL_ShowCursor(0);

		bool quit = false;

		MapEditor editor(system);

		while (!quit)
		{
			//Handle events on queue
			SDL_Event e;

			while (SDL_PollEvent(&e) != 0)
			{
				if (!editor.HandleEvent(e))
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
				}
			}

			// Update state
			editor.Update();

			//Clear screen
			SDL_SetRenderDrawColor(system.GetRenderer(), 84, 200, 255, 255);
			SDL_RenderClear(system.GetRenderer());
			SDL_SetRenderDrawColor(system.GetRenderer(), 0, 0, 0, 255);

			//Draw 
			editor.Draw(system.GetRenderer());

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
