#pragma once
#include <vector>
#include "Vector3.h"
#include "Vector2.h"
#include "SDL.h"

struct triangle
{
	Vector3f points[3];		
	double w[3];		//w point
	Vector3f viewpoints[3];
	Vector3f worldpoints[3];
	Vector3f v_normal[3];	//vertex normal
	Vector3f s_normal;	//surface normal
	Uint32 color;		//difsue lightcolor

	float ww;
	Vector3f norm_end[3];
	//lightcolor attribs
	Uint32 vertex_colors[3];
};
