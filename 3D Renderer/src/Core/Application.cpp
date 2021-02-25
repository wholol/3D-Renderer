#include "Application.h"
#include <string>
#include <iostream>
#include <thread>
#include <SDL.h>
#include "imgui.h"
#include "imgui_sdl.h"
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

	//load model.
	model.loadFromFile("suzanne.obj");
	
	//projection matrix setup
	vs.setProjectionMatrix(90.0f, 1.0f, 50.0f, SCREENHEIGHT, SCREENWIDTH);

	window = SDL_CreateWindow(title.c_str(), xpos, ypos, ScreenWidth, ScreenHeight, fullScreenflag);
	surface = SDL_GetWindowSurface(window);

	//lighting setup
	pl = std::make_shared<PointLightSetup>();	//create a new light source
	pl->setAmbient(0.1f);
	pl->setAttenuation(0.01f, 0.5f, 0.382f);
	pl->setSpecular(1.0f, 5.0f);
	pl->setLightPos({ 0.0f, 0.0f, -10.0f });
	

	dl = std::make_shared<DirectionalLightSetup>();
	dl->setAmbient(0.1f);
	dl->setLightDir({ 0.0f , 0.0f , -1.0f });

	curr_light = dl;	//initialzi curr light.

	//renderer and texture setup
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	
	//imgui setup
	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, screenwidth, screenheight);
}

void Application::Render()
{
	fs.Process(surface, model.vertexnormbuffer, vs.getRasterTriangles(), Object_color, curr_light, cam, vs.ProjMat, vs.ViewMat , renderWireFrame , renderVertexNormals);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	gui.RenderGUI(pl, dl,curr_light, model, surface , renderWireFrame , renderVertexNormals,Object_color);
	SDL_RenderPresent(renderer);
}

void Application::Update()
{
	gui.updateGUI();

	rotateX += 0.002f;
	rotateZ += 0.002f;
	Mat3f transform = Mat3f::Translate(0, 0, 3);

	transform = transform * Mat3f::RotateZ(rotateZ);
	transform = transform * Mat3f::RotateX(rotateX);

	vs.setViewMatrix(cam, lookDir);
	vs.setTransformMatrix(transform);
	vs.ProcessPrimitive(model.indexbuffer, model.vertexbuffer, model.vertexnormbuffer, curr_light,Object_color);

	//controls
	
	const Uint8 *kstate = SDL_GetKeyboardState(nullptr);
	
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

	SDL_RenderClear(renderer);
	SDL_FillRect(surface, 0, 0);
	SDL_DestroyTexture(texture);
}

bool Application::Quit()
{
	SDL_PollEvent(&event);
	
	switch (event.type)
	{
	case SDL_QUIT:
		ImGuiSDL::Deinitialize();
		ImGui::DestroyContext();
		return true;
	}
	return false;
}
