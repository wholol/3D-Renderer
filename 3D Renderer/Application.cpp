#include "Application.h"
#include <string>
#include <iostream>
#include <thread>
#include "SDL.h"
#include "Draw.h"


Application::Application(const std::string& title, int xpos, int ypos, int ScreenWidth, int ScreenHeight, bool fullscreen)
	:screenheight(ScreenHeight) , screenwidth(ScreenWidth)
{
	if (SDL_INIT_EVERYTHING < 0) {
		std::cout << "SDL could not initialize." << SDL_GetError() << std::endl;
	}

	unsigned int fullScreenflag = 0;
	if (fullscreen)
	{
		fullScreenflag = SDL_WINDOW_FULLSCREEN;
	}

	window = SDL_CreateWindow(title.c_str(), xpos, ypos, ScreenWidth, ScreenHeight, fullScreenflag);

	if (!window) {
		throw std::runtime_error("window initialization failed.");
	}

	
	surface = SDL_GetWindowSurface(window);
	testpoints = //South
	{
		//South
		{Vector3f(0 ,0 ,0) , Vector3f(0 ,1 ,0) , Vector3f(1 ,1 ,0)},
		{Vector3f(0 ,0 ,0) , Vector3f(1 ,1 ,0) , Vector3f(1 ,0 ,0)},

		//East
		{Vector3f(1 ,0 ,0) , Vector3f(1 ,1 ,0) ,Vector3f(1 ,1 ,1)},
		{Vector3f(1 ,0 ,0) , Vector3f(1 ,1 ,1) , Vector3f(1 ,0 ,1)},

		//north
		{Vector3f(1 ,0 ,1) , Vector3f(1 ,1 ,1) ,Vector3f(0 ,1 ,1)},
		{Vector3f(1 ,0 ,1) , Vector3f(0 ,1 ,1) , Vector3f(0 ,0 ,1)},

		//West
		{Vector3f(0 ,0 ,1) , Vector3f(0 ,1 ,1) , Vector3f(0 ,1 ,0)},
		{Vector3f(0 ,0 ,1) , Vector3f(0 ,1 ,0) , Vector3f(0 ,0 ,0)},

		//Top
		{Vector3f(0 ,1 ,0) , Vector3f(0 ,1 ,1) , Vector3f(1 ,1 ,1)},
		{Vector3f(0 ,1 ,0) ,Vector3f(1 ,1 ,1) , Vector3f(1 ,1 ,0)},

		//Bottom
		{Vector3f(1 ,0 ,1) , Vector3f(0 ,0 ,1) , Vector3f(0 ,0 ,0)},
		{Vector3f(1 ,0 ,1) , Vector3f(0 ,0 ,0) , Vector3f(1 ,0 ,0)},
	};
	//model.loadFromFile(testpoints, m);
	//model.loadFromFile("teapot.obj", m);
	model.loadFromFile("sphere.obj");

}

void Application::Render()
{
	Mat3f transform = Mat3f::Translate(0, 0, 5);
	transform = transform * Mat3f::RotateZ(rotateY);
	transform = transform * Mat3f::RotateX(rotateX);
		
	pipeline.setTransformations(transform);
	pipeline.setCamera(cam, lookDir);
	pipeline.setProjectionParams(90.0f, 0.1f, 1000.0f, screenheight, screenwidth);
	pipeline.Render(m , model.indexbuffer , model.vertexbuffer , model.vertexnormbuffer);

	rotateX += 0.001;
	rotateY += 0.001;
	
	pipeline.Draw(surface, model.vertexnormbuffer, SDL_MapRGB(surface->format, 200, 156, 255));

}

void Application::Update()
{
	const Uint8 *kstate = SDL_GetKeyboardState(NULL);
	
	if (kstate[SDL_SCANCODE_RIGHT]) {
		rotateY += 0.001;
	}


	if (kstate[SDL_SCANCODE_W]) {
		fYaw += 0.0012f;
		cam.y += 0.005f;
		// = Mat3f::RotateZ(fYaw) * lookDir;
	}

	if (kstate[SDL_SCANCODE_A]) {
		fYaw -= 0.0002f;
		cam.y -= 0.005f;
		//lookDir = Mat3f::RotateY(fYaw) * lookDir;
		
	}

	if (kstate[SDL_SCANCODE_G]) {
		pipeline.testfunc();

	}

	
	surface = SDL_GetWindowSurface(window);

	SDL_UpdateWindowSurface(window);
	SDL_FillRect(surface, 0, 0);

	
}

bool Application::Quit()
{
	SDL_PollEvent(&event);
	switch (event.type)
	{
	case SDL_QUIT:
		return true;
	}
	return false;
}
