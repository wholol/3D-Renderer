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
		
		vertexnormbuffer[a] += line2.getCrossProduct(line1);	//accumualte the normals for each vertex for specular lighting calcs
		vertexnormbuffer[b] += line2.getCrossProduct(line1);
		vertexnormbuffer[c] += line2.getCrossProduct(line1);

		triangle temp;
		temp.normal = line2.getCrossProduct(line1).getNormalized();
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
	}
	/*
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

	}*/
	
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
	Uint8 diffuse_rgba[4];
	Uint8 p1_vertex_rgba[4];
	Uint8 p2_vertex_rgba[4];
	Uint8 p3_vertex_rgba[4];

	for (int i = 0; i < rastertriangles.size(); ++i)
	{
		//extract rgba components of light colour
		Uint8 lightcol[4];
		lightcol[0] = (color & 0xFF000000) >> 24;
		lightcol[1] = (color & 0x00FF0000) >> 16;
		lightcol[2] = (color & 0x0000FF00) >> 8;
		lightcol[3] = (color & 0x000000FF);

		//use to access vertex normal buffer
		int p1_index = rastertriangles[i].index[0];
		int p2_index = rastertriangles[i].index[1];
		int p3_index = rastertriangles[i].index[2];

		//get the light intensity at the vertex
		double p1_vertex_t = DiffuseLightDir.getNormalized().getDotProduct(vertexnormbuffer[p1_index].getNormalized());
		if (p1_vertex_t < 0)
		{
			p1_vertex_t = 0.0;
		}
		double p2_vertex_t = DiffuseLightDir.getNormalized().getDotProduct(vertexnormbuffer[p2_index].getNormalized());
		if (p2_vertex_t < 0)
		{
			p2_vertex_t = 0.0;
		}
		double p3_vertex_t = DiffuseLightDir.getNormalized().getDotProduct(vertexnormbuffer[p3_index].getNormalized());
		if (p3_vertex_t < 0)
		{
			p3_vertex_t = 0.0;
		}

		p1_vertex_rgba[0] = lightcol[0];
		p2_vertex_rgba[1] = lightcol[0];
		p3_vertex_rgba[2] = lightcol[0];
		diffuse_rgba[0] = lightcol[0];

#define STORE_RGBA 1
		for (int j = 1; j < 4; ++j)
		{
			diffuse_rgba[j] = lightcol[j] * (rastertriangles[i].t);		//indiviual diffuse colour output
			p1_vertex_rgba[j] = lightcol[j] * p1_vertex_t;		//indiviual diffuse colour output
			p2_vertex_rgba[j] = lightcol[j] * p2_vertex_t;		//indiviual diffuse colour output
			p3_vertex_rgba[j] = lightcol[j] * p3_vertex_t;		//indiviual diffuse colour output

#if STORE_RGBA 
			rastertriangles[i].p1_rgba[j] = p1_vertex_rgba[j];
			rastertriangles[i].p2_rgba[j] = p2_vertex_rgba[j];
			rastertriangles[i].p3_rgba[j] = p3_vertex_rgba[j];
#endif
		}

		Uint32 diffuse_color, p1_color, p2_color, p3_color;
		diffuse_color = (diffuse_rgba[0] << 24) + (diffuse_rgba[1] << 16) + (diffuse_rgba[2] << 8) + diffuse_rgba[3];
		
		p1_color = (p1_vertex_rgba[0] << 24) + (p1_vertex_rgba[1] << 16) + (p1_vertex_rgba[2] << 8) + p1_vertex_rgba[3];
		p2_color = (p2_vertex_rgba[0] << 24) + (p2_vertex_rgba[1] << 16) + (p2_vertex_rgba[2] << 8) + p2_vertex_rgba[3];
		p3_color = (p3_vertex_rgba[0] << 24) + (p3_vertex_rgba[1] << 16) + (p3_vertex_rgba[2] << 8) + p3_vertex_rgba[3];
		
		rastertriangles[i].color = diffuse_color;
		rastertriangles[i].p1_color = p1_color;
		rastertriangles[i].p2_color = p2_color;
		rastertriangles[i].p3_color = p3_color;
	}

	for (int i  = 0 ; i < rastertriangles.size(); ++i)
	{	
		double p1_x = rastertriangles[i].points[0].x;
		double p2_x = rastertriangles[i].points[1].x;
		double p3_x = rastertriangles[i].points[2].x;

		double p1_y = rastertriangles[i].points[0].y;
		double p2_y = rastertriangles[i].points[1].y;
		double p3_y = rastertriangles[i].points[2].y;

		// plane equation parameters
		float d = ( rastertriangles[i].points[0].getDotProduct(rastertriangles[i].normal) );
		float a_prime = rastertriangles[i].normal.x / d;
		float b_prime = rastertriangles[i].normal.y / d;
		float c_prime = rastertriangles[i].normal.z / d;
		float w = rastertriangles[i].w;

		Draw::filltriangle(surface, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, w , a_prime, b_prime , c_prime , d ,ZBuffer, vertexnormbuffer , rastertriangles[i].p1_color, 
			rastertriangles[i].p2_color, rastertriangles[i].p3_color, rastertriangles[i].color);
		
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
	float n = tri.normal.getDotProduct(tri.points[0] - camerapos);
	
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
	if (t > 1.0) t = t;
	tri.t = t;

}