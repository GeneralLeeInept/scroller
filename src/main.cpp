#include "stdafx.h"

#include "Fixed.h"
#include "GameController.h"
#include "Input.h"
#include "System.h"

int main(int argc, char** argv)
{
	try
	{
		System system("Ryanalot's Adventure");
		Input input;
		GameController gameController(system, input);

		SDL_ShowCursor(0);
		gameController.Run();
	}
	catch (exception& e)
	{
		cout << "Error: " << e.what() << endl;
		return 1;
	}

	return 0;
}
