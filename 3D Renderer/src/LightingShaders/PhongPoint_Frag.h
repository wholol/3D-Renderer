#pragma once
#include "SDL.h"
#include "Draw.h"
#include <vector>
#include "Mat.h"


class PhongPoint_Frag
{
public:
	static void filltriangle_phong_point(SDL_Surface* surface, double p1_x, double p1_y, double p1_z, double p1_w, double p2_x, double p2_y, double p2_z, double p2_w, double p3_x, double p3_y, double p3_z, double p3_w, Vector3f& camerapos,
		std::vector<double>& ZBuffer, Mat3f Mat, Vector3f v1_vertex, Vector3f v2_vertex, Vector3f v3_vertex, PointLightSetup& pl, Uint32 color = 0xFFFFFF);

private:
	static void fillflatbottomtriangle_phong_point(SDL_Surface* surface, double p1_x, double p1_y, double p1_z, double p1_w, double p2_x, double p2_y, double p2_z, double p2_w, double p4_x, double p3_y, double p4_z, double p4_w, std::vector<double>& ZBuffer,
		Mat3f& Mat, Vector3f& camerapos, Vector3f p1_vertex, Vector3f p2_vertex, Vector3f p4_vertex, PointLightSetup& pl,
		Uint32 color = 0xFFFFFF);


	static void fillflattoptriangle_phong_point(SDL_Surface* surface, double p2_x, double p2_y, double p2_z, double p2_w, double p4_x, double p1_y, double p4_z, double p4_w, double p3_x, double p3_y, double p3_z, double p3_w, std::vector<double>&  ZBuffer,
		Mat3f& Mat, Vector3f& camerapos, Vector3f p4_vertex, Vector3f p2_vertex, Vector3f p3_vertex, PointLightSetup& pl,
		Uint32 color = 0xFFFFFF);

};