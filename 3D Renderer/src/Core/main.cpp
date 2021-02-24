#define TINYOBJLOADER_IMPLEMENTATION 
//#define TINYOBJLOADER_USE_DOUBLE
#include "Application.h"
#include "SDL.h"
#include "ScreenSize.h"

int main(int argc, char* argv[])
{
	Application a("title", 500, 200, SCREENWIDTH, SCREENHEIGHT, 0);
	
	while (!a.Quit())
	{
		a.Update();
		a.Render();
	}
	return 0;
}