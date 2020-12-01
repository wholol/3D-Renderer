#include "Pipeline.h"
#include "Draw.h"
#include "Timer.h"
#include <algorithm>
#include <chrono>
#include <iostream>

void Pipeline::setProjectionParams(float FovDegrees, float Near, float Far, unsigned int ScreenHeight, unsigned int ScreenWidth)
{
	float AspectRatio = ((float)ScreenHeight / (float)ScreenWidth);
	this->ScreenHeight = ScreenHeight;
	this->ScreenWidth = ScreenWidth;
	ProjMat = Mat3f::Projection(Near, Far, AspectRatio, FovDegrees);
	
	ZBuffer.reserve(ScreenHeight * ScreenWidth);

	for (int i = 0; i < ScreenHeight * ScreenWidth; ++i)
	{
		ZBuffer.emplace_back(INFINITY);
	}
}

void Pipeline::setTransformations(const Mat3f finalTransform)
{
	ModelMat = finalTransform;
}


void Pipeline::setCamera(Vector3f& camerapos, Vector3f& lookDir)
{
	ViewMat = Mat3f::CameraPointAt(camerapos, lookDir);
	ViewMat = Mat3f::InverseCamera(ViewMat);
	this->camerapos = camerapos;
}

void Pipeline::Render(mesh& m, std::vector<int>& indexbuffer, std::vector<Vector3f>& vertexbuffer, std::vector<Vector3f>& vertexnormbuffer, bool testforcull)
{
	/*
	for (int i = 0; i < indexbuffer.size(); i += 3)
	{
	
		int a = indexbuffer[i];
		int b = indexbuffer[i + 1];
		int c = indexbuffer[i + 2];

		Vector3f p1 = vertexbuffer[a];
		Vector3f p2 = vertexbuffer[b];
		Vector3f p3 = vertexbuffer[c];

		//model to world transform
		p1 = ModelMat * p1;
		p2 = ModelMat * p2;
		p3 = ModelMat * p3;

		Vector3f line1 = p3 - p1;
		Vector3f line2 = p2 - p1;
		
		vertexnormbuffer[a] += line1.getCrossProduct(line2);	//accumualte the normals for each vertex for specular lighting calcs
		vertexnormbuffer[b] += line1.getCrossProduct(line2);
		vertexnormbuffer[c] += line1.getCrossProduct(line2);

		triangle temp;
		temp.points[0] = p1;
		temp.points[1] = p2;
		temp.points[2] = p3;
		temp.index[0] = a;	
		temp.index[1] = b;
		temp.index[2] = c;

		if (!cullTriangle(temp, camerapos))			//if the triangle does not need to be culled, proceed
		{
			
			WorldtoCameraTransform(temp);
			setDiffuseLight(temp);	//only calculate if the triangle is seen by the camera
			//clipping here
			NDCTransform(temp);
			ViewPortTransform(temp);
			rastertriangles.emplace_back(temp);
		}
	}*/
	
	for (auto& tri : m.triangles)
	{
		triangle temp;
		ModeltoWorldTransform(tri, temp);
		if (!cullTriangle(temp, camerapos))			//if the triangle does not need to be culled, proceed
		{

			WorldtoCameraTransform(temp);
			setDiffuseLight(temp);	//only calculate if the triangle is seen by the camera
			//clipping here
			NDCTransform(temp);
			ViewPortTransform(temp);
			rastertriangles.emplace_back(temp);
		}

	}
	
	//vertex norm buffer completes calcualtions after the loop.
	//Access vertex buffer manudally in frag sahder

	//perform model to world transform first.
	// for i .. i += 3 in index buffer 
		
	//covert to world coordiantes

	//check culling
	//if not cull

		//generate temp vector3d for each vertex with i  
			//calculate diffuse value + ambient  
			//transform temp vec3d 
			//output positino vectors and lighting
				
			//fragment shader
				//contruct triangle with vector 3ds
				//z buffering time...
				//use i for coomputed vertex normal points for specular lighting..?
				//get lighting color
}

void Pipeline::Draw(SDL_Surface* surface, std::vector<Vector3f>& vertexnormbuffer, Uint32 color,bool wireframe)
{
	Uint8 modified_col[4];
	sortZDirection();
	for (int i = 0; i < rastertriangles.size(); ++i)
	{
		Uint8 lightcol[4];
		lightcol[0] = (color & 0xFF000000) >> 24;
		lightcol[1] = (color & 0x00FF0000) >> 16;
		lightcol[2] = (color & 0x0000FF00) >> 8;
		lightcol[3] = (color & 0x000000FF);

		

		for (int j = 1; j < 4; ++j)
		{
			modified_col[j] = lightcol[j] * rastertriangles[i].t;

		}
		Uint32 newcolor;
		newcolor = (modified_col[0] << 24) + (modified_col[1] << 16) + (modified_col[2] << 8) + modified_col[3];
		rastertriangles[i].color = newcolor;
	}

	for (int i  = 0 ; i < rastertriangles.size(); ++i)
	{
		
		double p1_x = rastertriangles[i].points[0].x;
		double p2_x = rastertriangles[i].points[1].x;
		double p3_x = rastertriangles[i].points[2].x;

		double p1_y = rastertriangles[i].points[0].y;
		double p2_y = rastertriangles[i].points[1].y;
		double p3_y = rastertriangles[i].points[2].y;

		int p1_index = rastertriangles[i].index[0];		
		int p2_index = rastertriangles[i].index[1];		
		int p3_index = rastertriangles[i].index[2];		
		
		// plane equation parameters
		float d = ( rastertriangles[i].points[0].getDotProduct(rastertriangles[i].normal) );
		float a_prime = rastertriangles[i].normal.x / d;
		float b_prime = rastertriangles[i].normal.y / d;
		float c_prime = rastertriangles[i].normal.z / d;
		float w = rastertriangles[i].w;
		

		

		Draw::filltriangle(surface, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, w , a_prime, b_prime , c_prime , d ,ZBuffer, rastertriangles[i].color);
		
		Draw::drawtriangle(surface, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d,
			ZBuffer,SDL_MapRGB(surface->format, 0, 0, 255));		//wrirefram colour to be black
	}

	rastertriangles.clear();
	for (int i = 0; i < vertexnormbuffer.size(); ++i)
	{
		vertexnormbuffer[i] = { 0 ,0 ,0};
	}

	ZBuffer.clear();
}

void Pipeline::testfunc()
{
}


void Pipeline::ModeltoWorldTransform(triangle& original,triangle& output)
{
	for (int i = 0; i < 3; ++i) {
		output.points[i] = ModelMat * original.points[i];
	}
}

bool Pipeline::cullTriangle(triangle& tri, Vector3f& camerapos)
{
	Vector3f triangleline1 = tri.points[1] - tri.points[0];
	Vector3f triangleline2 = tri.points[2] - tri.points[0];
	Vector3f triangleNormal = triangleline1.getCrossProduct(triangleline2);
	tri.normal = triangleNormal.Normalize();
	float n = triangleNormal.getDotProduct(tri.points[0] - camerapos);
	
	if (n < 0.0f) {
		return false;
	}
	return true;
}

void Pipeline::WorldtoCameraTransform(triangle& tri)
{
	for (int i = 0; i < 3; ++i) {
		tri.points[i] = ViewMat * tri.points[i];
	}
}

void Pipeline::NDCTransform(triangle& tri)
{
	for (int i = 0; i < 3; ++i) {
		tri.points[i] = ProjMat * tri.points[i];
		tri.points[i] = Mat3f::Normalize(tri.points[i], ProjMat);
		tri.w = ProjMat.w;				//store the w value of the projection matrix
	}
}

void Pipeline::sortZDirection()
{
	auto sortZ = [](triangle& t1, triangle& t2) {
		float z1 = 0.0f, z2 = 0.0f;

		for (int i = 0; i < 3; ++i)
		{
			 z1 += t1.points[i].z;
			 z2 += t2.points[i].z;
		}

		return z2 / 3 > z1 / 3;
	};

	std::sort(rastertriangles.begin(), rastertriangles.end(), sortZ);
}

void Pipeline::ViewPortTransform(triangle& tri)
{
	for (int i = 0; i < 3; ++i)
	{
		tri.points[i] = Mat3f::Translate(1, 1, 0) * tri.points[i];

		tri.points[i] = Mat3f::Scale(0.5 * ScreenWidth, 0.5 * ScreenHeight, 1) * tri.points[i];
	}
}

void Pipeline::setDiffuseLight(triangle& tri)
{

	//lighting equations:
	//http://www.conitec.net/shaders/shader_work2.html
	//Diffuse Light = Diffuse Intensity * N · L
	//ambient  = constant
	//Final Color = (Diffuse Light + Ambient Light) * Diffuse Color
	
	
	double t =  DiffuseLightDir.getNormalized().getDotProduct(tri.normal) ;		//assime ambietn coefficnet 0.5 for r g and b
	
	if (t < 0.0) t = 0;
	//else if (t < 0.2) t = 0.2;
	//else if (t < 0.4) t = 0.4;
	//else if (t < 0.6) t = 0.6;
	//else if (t < 0.8) t = 0.8;
	if (t > 1.0) t = 1;
	tri.t = t;

}