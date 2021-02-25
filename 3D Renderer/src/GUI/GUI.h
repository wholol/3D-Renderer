#pragma once
#include "imgui.h"
#include "imgui_sdl.h"
#include "LoadModel.h"
#include "Diffuse_type.h"
#include "Light_Type.h"
#include "LightSetup.h"


class GUI
{
public:
	void RenderGUI(std::shared_ptr<PointLightSetup>& pl , std::shared_ptr<DirectionalLightSetup>& dl,std::shared_ptr<Light>& curr_light, LoadModel& model, SDL_Surface* surface , bool& wireframe,bool& vertexnormals , Uint32& ObjectCol);
	void updateGUI();

private:
	bool flat_shading = true;
	bool phong_shading = true;
	bool gouraud_shading = true;
	bool bPointLight = true;
	bool bDirectLight = false;
	ImVec4 lightcolor_gui = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
	ImVec4 objectcolor_gui = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	//models
	bool sphere_model = false;
	bool cube_model = false;
	bool suzanne_model = false;
	bool axis_model = false;
	bool teapot_model = true;
	bool wireframe = false;
	bool vertexnormals = false;

	


};