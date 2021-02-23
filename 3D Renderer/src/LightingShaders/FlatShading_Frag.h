#pragma once
#include "SDL.h"
#include "Draw.h"
#include <vector>
class FlatShading_Frag
{
public:
	static void filltriangle_flat(SDL_Surface* surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p3_x, double p3_y, double p3_z, std::vector<double>& ZBuffer, Uint32 color);
	
private:

	static void fillflatbottomtriangle_flat(SDL_Surface* surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p4_x, double p3_y, double p4_z, std::vector<double>& ZBuffer
		, Uint32 color);

	static void fillflattoptriangle_flat(SDL_Surface* surface, double p2_x, double p2_y, double p2_z, double p4_x, double p1_y, double p4_z, double p3_x, double p3_y, double p3_z, std::vector<double>& ZBuffer,
		Uint32 color);

};