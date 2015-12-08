#include "stdafx.h"

#include "Fixed.h"
#include "GameController.h"
#include "System.h"

void TestFixed()
{
	Fixed a, b;
	b = (3 << 6) + 48;// 1;// (1 << 6);// +(64 >> 2);
	a = (1 << 6) + 12;// (64 << 6);// +(64 >> 2);
	cout << FixedFormat(a) << " + " << FixedFormat(b) << " = " << FixedFormat(FixedAdd(a, b)) << endl;
	cout << FixedFormat(a) << " - " << FixedFormat(b) << " = " << FixedFormat(FixedSub(a, b)) << endl;
	cout << FixedFormat(a) << " / " << FixedFormat(b) << " = " << FixedFormat(FixedDiv(a, b)) << endl;
	cout << FixedFormat(a) << " * " << FixedFormat(b) << " = " << FixedFormat(FixedMul(a, b)) << endl;
}

int main(int argc, char** argv)
{
	TestFixed();

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
