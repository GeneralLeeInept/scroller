#include "stdafx.h"

#include "GameController.h"
#include "System.h"

int main(int argc, char** argv)
{
	try
	{
		System system("Ryanalot's Adventure");
		SDL_ShowCursor(0);

		GameController gameController(system);
		gameController.Run();
	}
	catch (exception& e)
	{
		cout << "Error: " << e.what() << endl;
		return 1;
	}

	return 0;
}
