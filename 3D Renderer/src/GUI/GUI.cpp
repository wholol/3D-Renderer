#include "GUI.h"

void GUI::RenderGUI(std::shared_ptr<PointLightSetup>& pl, std::shared_ptr<DirectionalLightSetup>& dl, std::shared_ptr<Light>& curr_light, LoadModel& model,SDL_Surface* surface , bool& wireframe, bool& vertexnormals , Uint32& objectcolor)
{
	{
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

		//lightcolor
		ImGui::Text("Light Color");
		ImGui::ColorEdit3("light color", (float*)&lightcolor_gui); // Edit 3 floats representing a lightcolor
		int r_light = (int)(lightcolor_gui.x * 255);
		int g_light = (int)(lightcolor_gui.y * 255);
		int b_light = (int)(lightcolor_gui.z * 255);
		curr_light->setLightCol(SDL_MapRGB(surface->format , r_light , g_light , b_light ));

		//object color
		ImGui::Text("Object Color");
		ImGui::ColorEdit3("object color", (float*)&objectcolor_gui); // Edit 3 floats representing a lightcolor
		int r_object = (int)(objectcolor_gui.x * 255);
		int g_object = (int)(objectcolor_gui.y * 255);
		int b_object = (int)(objectcolor_gui.z * 255);
		objectcolor = SDL_MapRGB(surface->format, r_object, g_object, b_object);

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
		
		
		ImGui::Checkbox("WireFrame", &(this->wireframe));
		wireframe = this->wireframe;

		ImGui::Checkbox("vertex normals", &(this->vertexnormals));
		vertexnormals = this->vertexnormals;


		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}



	//render GUI
	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());
}

void GUI::updateGUI()
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

}
