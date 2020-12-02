#pragma once
#include <vector>
#include "Vector3.h"
#include "Vector2.h"

struct triangle
{
	Vector3f points[3];		
	Vector3f worldpoints[3];
	int index[3];	// index that correspods to the vertex points.
	Vector2f textCoord[3];
	Vector3f normal;
	Uint32 color;		//difsue color
	double t;		//for lighting

	//colors for the vertex
	Uint32 p1_color;
	Uint32 p2_color;
	Uint32 p3_color;

	//store rgba
	Uint8 p1_rgba[4];
	Uint8 p2_rgba[4];
	Uint8 p3_rgba[4];

	//homogenoues w value for the triangle
	float w;		
	
};

struct mesh
{
	std::vector<triangle> triangles;
};