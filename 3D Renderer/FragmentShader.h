#pragma once
#include "SDL.h"
#include <vector>
#include "Vector3.h"
#include "LightSetup.h"
#include "triangle.h"

class FragmentShader
{
public:
	void Process(SDL_Surface* surface, std::vector<Vector3f>& vertexnormbuffer, std::vector<triangle>& rastertriangles, Uint32 objcolor, std::shared_ptr<Light> light, bool wireframe = true, bool draw_normals = false);
private:
	std::vector<double> ZBuffer;
};