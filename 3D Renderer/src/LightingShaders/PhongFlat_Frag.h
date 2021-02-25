#pragma once
#include "SDL.h"
#include "Vector3.h"
#include "Draw.h"
#include <vector>

class PhongFlat_Frag
{
public:
	static void filltriangle_phong_flat(SDL_Surface* surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p3_x, double p3_y, double p3_z,
		std::vector<double>& ZBuffer, Vector3f v1_vertex, Vector3f v2_vertex, Vector3f v3_vertex, DirectionalLightSetup& dl, Uint32 color = 0xFFFFFF);

private:
	static void fillflatbottomtriangle_phong_flat(SDL_Surface* surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p4_x, double p3_y, double p4_z, std::vector<double>& ZBuffer,
		Vector3f p1_vertex, Vector3f p2_vertex, Vector3f p4_vertex, DirectionalLightSetup& dl, Uint32 color = 0xFFFFFF);


	static void fillflattoptriangle_phong_flat(SDL_Surface* surface, double p2_x, double p2_y, double p2_z, double p4_x, double p1_y, double p4_z, double p3_x, double p3_y, double p3_z, std::vector<double>& ZBuffer, 
		Vector3f p4_vertex, Vector3f p2_vertex, Vector3f p3_vertex, DirectionalLightSetup& dl, Uint32 color = 0xFFFFFF);

};