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
	pl->setDiffuse(Diffuse_Type::Phong_Shading);
	pl->setAttenuation(0.01f, 0.5f, 0.382f);
	pl->setLightCol(SDL_MapRGB(surface->format, 200, 255, 255));

	pl->setSpecular(1.0f, 5.0f);
	pl->setLightPos({ 0.0f, 0.0f, -10.0f });
	pl->setLightCol(SDL_MapRGB(surface->format, 200, 255, 255));

	dl = std::make_shared<DirectionalLightSetup>();
	dl->setAmbient(0.1f);
	dl->setDiffuse(Diffuse_Type::Gouraud_Shading);
	dl->setLightDir({ 0.0f , 0.0f , -1.0f });
	dl->setLightCol(SDL_MapRGB(surface->format, 200, 255, 255));

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
	fs.Process(surface, model.vertexnormbuffer, vs.getRasterTriangles(), SDL_MapRGB(surface->format, 200, 255, 255), curr_light, cam, vs.ProjMat, vs.ViewMat);	
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);

	{
		static bool flat_shading = true;
		static bool phong_shading = true;
		static bool gouraud_shading = true;
		static bool bPointLight = true;
		static bool bDirectLight = false;
		static ImVec4 color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		//models
		static bool sphere_model = false;
		static bool cube_model = false;
		static bool suzanne_model = false;
		static bool axis_model = false;
		static bool teapot_model = true;

		static bool wireframe = false;
		
		ImGui::Begin("GUI");
		ImGui::Text("Light Type");
		ImGui::Checkbox("DirectLight", &bDirectLight);
		if (bDirectLight)
		{
			bPointLight = false;
			curr_light = dl;
			ImGui::SliderFloat("dir_x", &dl->lightdir.x, -1.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("dir_y", &dl->lightdir.y, -1.0f, 1.0f);
			ImGui::SliderFloat("dir_z", &dl->lightdir.z, -1.0f, 1.0f);
		}

		ImGui::Checkbox("PointLight", &bPointLight);      // Edit bools storing our window open/close state
		if (bPointLight)
		{
			bDirectLight = false;
			curr_light = pl;
			ImGui::SliderFloat("posx", &pl->lightpos.x, -10.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("posy", &pl->lightpos.y, -10.0f, 10.0f);
			ImGui::SliderFloat("posz", &pl->lightpos.z, -10.0f, 10.0f);
			ImGui::SliderFloat("specular shininess", &pl->spec_exponent, 1.0f, 128.0f);
			ImGui::SliderFloat("specular intensity", &pl->spec_intensity, 0.0f, 60.0f);
		}

		//color
		ImGui::ColorEdit3("color", (float*)&color); // Edit 3 floats representing a color
		int r = (int)(color.x * 255);
		int g = (int)(color.y * 255);
		int b = (int)(color.z * 255);
		curr_light->setLightCol(SDL_MapRGB(surface->format, r, g, b));

		//shading types
		ImGui::Text("Shading Type");
		ImGui::Checkbox("Flat shading", &flat_shading);
		if (flat_shading)
		{
			gouraud_shading = false;
			phong_shading = false;
			curr_light->setDiffuse(Diffuse_Type::Flat_Shading);
		}

		ImGui::Checkbox("Gouraud shading", &gouraud_shading);
		if (gouraud_shading)
		{
			flat_shading = false;
			phong_shading = false;
			curr_light->setDiffuse(Diffuse_Type::Gouraud_Shading);
		}

		ImGui::Checkbox("Phong shading", &phong_shading);
		if (phong_shading)
		{
			flat_shading = false;
			gouraud_shading = false;
			curr_light->setDiffuse(Diffuse_Type::Phong_Shading);
		}

		//model loaders
		ImGui::Text("Models");
		ImGui::Checkbox("Suzanne", &suzanne_model);
		if (suzanne_model)
		{
			model.loadFromFile("suzanne.obj");
			suzanne_model = true;
			sphere_model = false;
			cube_model = false;
			axis_model = false;
			teapot_model = false;
		}

		ImGui::Checkbox("Sphere", &sphere_model);
		if (sphere_model)
		{
			model.loadFromFile("sphere.obj");
			suzanne_model = false;
			sphere_model = true;
			cube_model = false;
			axis_model = false;
			teapot_model = false;
		}

		ImGui::Checkbox("Cube", &cube_model);
		if (cube_model)
		{
			model.loadFromFile("cube.obj");
			suzanne_model = false;
			sphere_model = false;
			cube_model = true;
			axis_model = false;
			teapot_model = false;
		}
		
		ImGui::Checkbox("Teapot", &teapot_model);
		if (teapot_model)
		{
			model.loadFromFile("teapot.obj");
			teapot_model = true;
			suzanne_model = false;
			sphere_model = false;
			cube_model = false;
			axis_model = false;
		}

		ImGui::Checkbox("WireFrame", &wireframe);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
	
	//render GUI
	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());
	
	SDL_RenderPresent(renderer);
}

void Application::Update()
{
	ImGui::NewFrame();
	//io setup
	ImGuiIO& io = ImGui::GetIO();
	int mouseX, mouseY, wheel = 0;
	const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
	io.DeltaTime = 1.0f / 60.0f;
	io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
	io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
	io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
	io.MouseWheel = static_cast<float>(wheel);
	
	rotateX += 0.002f;
	rotateZ += 0.002f;
	Mat3f transform = Mat3f::Translate(0, 0, 3);

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
