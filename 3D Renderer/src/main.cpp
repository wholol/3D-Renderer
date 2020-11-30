
#include <iostream>
#include "Application.h"


int main(int argc, char* argv[])
{
	Application a("title", 0, 0, 800, 600, 0);

	while (!a.Quit())
	{
		a.Update();
		a.Render();
	}
	return 0;
}

