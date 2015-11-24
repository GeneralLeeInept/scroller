#include "stdafx.h"

#include "GameController.h"
#include "MainMenu.h"
#include "System.h"

int main(int argc, char** argv)
{
	try
	{
		System system("Super Ryan Land");
		SDL_ShowCursor(0);

		GameController gameController(system);
		shared_ptr<IGameState> mainMenu;
		mainMenu.reset(new MainMenu(system, gameController));
		gameController.GotoState(mainMenu);
		gameController.Run();
	}
	catch (exception& e)
	{
		cout << "Error: " << e.what() << endl;
		return 1;
	}

	return 0;
}
