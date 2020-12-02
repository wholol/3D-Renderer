#pragma once
#include "SDL.h"
#include <string>
#include "LoadModel.h"
#include "Pipeline.h"
#include "mesh.h"

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
	SDL_Surface* surface;
	LoadModel model;
	SDL_Event event;
	mesh m;
	std::vector<triangle> testpoints;
	Pipeline pipeline;
	Vector3f lookDir = { 0, 0, 1 };
	Vector3f cam = { 0 , 0, 0 };
	unsigned int screenwidth, screenheight;
	float rotateX = 0.0f;
	float rotateY = 0.0f;
	float fYaw = 0.0f;
	int x = 20;
	int y = 20;
	const int fps = 60;
	int counter = 0;
};