#pragma once
#include "SDL.h"
#include <string>
#include "LoadModel.h"
#include "triangle.h"
#include "LightSetup.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "ScreenSize.h"
#include "imgui.h"

class Application
{
public:
	Application(const std::string& title, int xpos, int ypos, int ScreenWidth, int ScreenHeight, bool fullscreen);
	void Render();
	void Update();
	bool Quit();

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Surface* surface;
	SDL_Event event;

	LoadModel model;
	VertexShader vs;
	FragmentShader fs;
	
	std::shared_ptr<PointLightSetup> pl;		//point light setup
	std::shared_ptr<DirectionalLightSetup> dl;	//direct light setup
	std::shared_ptr<Light> curr_light;			

	Vector3f lookDir = { 0, 0, 1 };
	Vector3f cam = { 0 , 0, 0 };
	unsigned int screenwidth, screenheight;
	

	float rotateX = 0.0f;
	float rotateZ = 0.0f;
	float fYaw = 0.0f;
	int x = 20;
	int y = 20;
	const int fps = 60;
};