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
	
	model.loadFromFile("suzanne.obj");
	
	//projection matrix setup
	vs.setProjectionMatrix(90.0f, 1.0f, 50.0f, SCREENHEIGHT, SCREENWIDTH);

	//init lighting
	pl = std::make_shared<PointLightSetup>();	//create a new light source
	pl->setAmbient(0.1f);
	pl->setDiffuse(Diffuse_Type::Phong_Shading);
	pl->setAttenuation(0.01f, 0.5f, 0.382f);
	
	pl->setSpecular(0.7f ,10.0f);
	pl->setLightPos({ 0.0f, 0.0f, -10.0f });
	pl->setLightCol(SDL_MapRGB(surface->format, 200, 255, 255));

	dl = std::make_shared<DirectionalLightSetup>();
	dl->setAmbient(0.1f);
	dl->setDiffuse(Diffuse_Type::Gouraud_Shading);
	dl->setLightDir({ 0.0f , 0.0f , -1.0f });
	dl->setLightCol(SDL_MapRGB(surface->format, 200, 255, 255));	

	curr_light = dl;	//initialzi curr light.
}

void Application::Render()
{
	fs.Process(surface, model.vertexnormbuffer, vs.getRasterTriangles(), SDL_MapRGB(surface->format, 200, 255, 255), curr_light, cam, vs.ProjMat, vs.ViewMat);
}

void Application::Update()
{
	rotateX += 0.002;
	rotateZ += 0.002;
	Mat3f transform = Mat3f::Translate(0, 0, 5);
	transform = transform * Mat3f::RotateZ(rotateZ);
	transform = transform * Mat3f::RotateX(rotateX);

	vs.setViewMatrix(cam, lookDir);
	vs.setTransformMatrix(transform);
	vs.ProcessPrimitive(model.indexbuffer, model.vertexbuffer, model.vertexnormbuffer, curr_light);

	
	
	
	//controls
	
	const Uint8 *kstate = SDL_GetKeyboardState(NULL);
	
	if (kstate[SDL_SCANCODE_UP]) {
		
		cam.z += 0.005f;
	}

	if (kstate[SDL_SCANCODE_DOWN]) {
		cam.z -= 0.005f;
	}

	if (kstate[SDL_SCANCODE_D]) {
		fYaw = 0.0f;
		fYaw += 0.01f;
		lookDir =  Mat3f::RotateY(fYaw) * lookDir;
	}

	if (kstate[SDL_SCANCODE_A]) {
		fYaw = 0.0f;
		fYaw -= 0.01f;
		lookDir = Mat3f::RotateY(fYaw) * lookDir;
	}
	
	if (kstate[SDL_SCANCODE_W]) {
		cam.y += 0.05f;
	}

	if (kstate[SDL_SCANCODE_S]) {
		cam.y -= 0.05f;
	}

	if (kstate[SDL_SCANCODE_O]) {
		pl->lightpos += {0.0, -0.05f, 0.0f};
	}

	if (kstate[SDL_SCANCODE_T]) {
		curr_light = dl;
	}

	if (kstate[SDL_SCANCODE_R]) {
		curr_light = pl;
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
