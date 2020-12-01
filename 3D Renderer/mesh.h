#pragma once
#include <vector>
#include "Vector3.h"
#include "Vector2.h"

struct triangle
{
	Vector3f points[3];			
	int index[3];	// index that correspods to the vertex points.
	Vector2f textCoord[3];
	Vector3f normal;
	Uint32 color;
	double t;		//for lighting

	//homogenoues w value for the triangle
	float w;		
	
};

struct mesh
{
	std::vector<triangle> triangles;
};