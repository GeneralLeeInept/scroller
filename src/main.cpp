#include <iostream>

#include "System.h"

int main(int argc, char** argv)
{
	try
	{
		System system("Super Ryan World");
	}
	catch (std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
