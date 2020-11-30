#pragma once
#include <vector>
#include "Vector3.h"
#include "Vector2.h"

struct triangle
{
	Vector3f points[3];			
	int index[3];	// index that correspods to the points.
	Vector2f textCoord[3];
	Vector3f normal;
	Vector3f color;
	double t;		//for lighting
	
	//plane equation parameters to compute Zcam for each pixel
	float a_prime;
	float b_prime;
	float c_prime;
	float d;

	//homogenoues w value for the triangle
	float w;		
	
};

struct mesh
{
	std::vector<triangle> triangles;
};