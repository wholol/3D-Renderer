#pragma once
#include <vector>
#include "Vector3.h"
#include "Vector2.h"

struct triangle
{
	Vector3f points[3];		
	Vector3f worldpoints[3];
	
	int index[3];	// index that correspods to the vertex points.
	Vector3f v_normal[3];	//vertex normal
	Vector3f s_normal;	//surface normal
	Uint32 color;		//difsue color
	double t;		//for lighting

	Uint32 p1_color;
	Uint32 p2_color;
	Uint32 p3_color;
	//homogenoues w value for the triangle
	double w;

	//color attribs
	Uint32 vertex_colors[3];
	
};

struct mesh
{
	std::vector<triangle> triangles;
};