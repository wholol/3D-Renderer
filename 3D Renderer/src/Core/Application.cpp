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

	//imGUI_SDL setup
	window = SDL_CreateWindow(title.c_str(), xpos, ypos, ScreenWidth, ScreenHeight, fullScreenflag);

	//
	////imGUI setup
	//// // GL 3.0 + GLSL 130
	//const char* glsl_version = "#version 130";
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	//// Create window with graphics context
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	//SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	//SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	//window = SDL_CreateWindow(title.c_str(), xpos, ypos, ScreenWidth, ScreenHeight, window_flags);
	//SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	//SDL_GL_MakeCurrent(window, gl_context);
	//SDL_GL_SetSwapInterval(1); // Enable vsync
	//
	surface = SDL_GetWindowSurface(window);
	//
	//if (glewInit() != GLEW_OK)
	//{
	//	fprintf(stderr, "Failed to initialize OpenGL loader!\n");
	//}
	////
	////// Setup Dear ImGui context
	////
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//io = ImGui::GetIO();
	//(void)io;
	//
	//
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//
	//// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	//
	//// Setup Platform/Renderer backends
	//ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	//ImGui_ImplOpenGL3_Init(glsl_version);

	//lighting setup
	pl = std::make_shared<PointLightSetup>();	//create a new light source
	pl->setAmbient(0.1f);
	pl->setDiffuse(Diffuse_Type::Phong_Shading);
	pl->setAttenuation(0.01f, 0.5f, 0.382f);

	pl->setSpecular(0.7f, 10.0f);
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
	
	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, screenwidth, screenheight);
	//io = ImGui::GetIO();
	//(void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
}

void Application::Render()
{
	
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	
	fs.Process(surface, model.vertexnormbuffer, vs.getRasterTriangles(), SDL_MapRGB(surface->format, 200, 255, 255), curr_light, cam, vs.ProjMat, vs.ViewMat);	
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);

	{
		static int counter = 0;
		static bool flat_shading = true;
		static bool phong_shading = true;
		static bool gouraud_shading = true;
		static bool bPointLight = true;
		static bool bDirectLight = false;
		
		ImGui::Begin("GUI");


		//TODO: light color!
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
		}



		//shading types
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

	
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		
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
	
	SDL_RenderClear(renderer);
	SDL_FillRect(surface, 0, 0);
	SDL_DestroyTexture(texture);

}

bool Application::Quit()
{
	SDL_PollEvent(&event);
	//ImGui_ImplSDL2_ProcessEvent(&event);
	switch (event.type)
	{
	case SDL_QUIT:
		ImGuiSDL::Deinitialize();
		//ImGui_ImplOpenGL3_Shutdown();
		//ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		return true;
	}
	return false;
}
