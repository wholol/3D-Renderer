#include "Pipeline.h"
#include "Draw.h"
#include <algorithm>

void Pipeline::setProjectionParams(float FovDegrees, float Near, float Far, unsigned int ScreenHeight, unsigned int ScreenWidth)
{
	float AspectRatio = ((float)ScreenHeight / (float)ScreenWidth);
	this->ScreenHeight = ScreenHeight;
	this->ScreenWidth = ScreenWidth;
	ProjMat = Mat3f::Projection(Near, Far, AspectRatio, FovDegrees);
	
	ZBuffer.reserve(ScreenHeight * ScreenWidth);

	for (auto &zbuffer : ZBuffer)
	{
		zbuffer = INFINITY;
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
			NDCTransform(temp);
			ViewPortTransform(temp);
			rastertriangles.emplace_back(temp);
		}
	}
	

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
	/*
	for (auto& triangles : m.triangles) {
		triangle temp;
		
		ModeltoWorldTransform(triangles,temp);			//model view projection transform on the triangles.
	
		if (testforcull)	//check if culling is enabled
		{		
		
			if (!cullTriangle(temp, camerapos))			//if the triangle does not need to be culled, proceed
			{
				setDiffuseLight(temp);	//only calculate if the triangle is seen by the camera
				WorldtoCameraTransform(temp);
				NDCTransform(temp);
				ViewPortTransform(temp); 
				rastertriangles.emplace_back(temp);
			}
		}

		else {		//if user ds not want culling
			setDiffuseLight(temp);	//only calculate if the triangle is seen by the camera
			WorldtoCameraTransform(temp);
			NDCTransform(temp);
			ViewPortTransform(temp);
			rastertriangles.emplace_back(temp);
		}
	}*/
}

void Pipeline::Draw(SDL_Surface* surface, std::vector<Vector3f>& vertexnormbuffer, Uint32 color,bool wireframe)
{
	sortZDirection();		//sort traingle z direction


	for (int i  = 0 ; i < rastertriangles.size(); ++i)
	{
		double x0 = rastertriangles[i].points[0].x;
		double x1 = rastertriangles[i].points[1].x;
		double x2 = rastertriangles[i].points[2].x;

		double y0 = rastertriangles[i].points[0].y;
		double y1 = rastertriangles[i].points[1].y;
		double y2 = rastertriangles[i].points[2].y;
		 
		// d = -(dot (point 0 , normal) )
		float d = -( rastertriangles[i].points[0].getDotProduct(rastertriangles[i].normal) );
		float a_prime = rastertriangles[i].normal.x / d;
		float b_prime = rastertriangles[i].normal.y / d;
		float c_prime = rastertriangles[i].normal.z / d;
		float w = rastertriangles[i].w;

		Uint8 rgba[4];
		rgba[0] = (color & 0xFF000000) >> 24;
		rgba[1] = (color & 0x00FF0000) >> 16;
		rgba[2] = (color & 0x0000FF00) >> 8;
		rgba[3] = (color & 0x000000FF);

		for (int j = 1; j < 4; ++j)
		{
			rgba[j] = (double)rgba[j] * rastertriangles[i].t;
		}

		color = (rgba[0] << 24) + (rgba[1] << 16) + (rgba[2] << 8) + rgba[3];

		Draw::filltriangle(surface, x0, y0, x1, y1,x2, y2, w , a_prime, b_prime , c_prime  , d ,ZBuffer, color);
		
		Draw::drawtriangle(surface, x0, y0, x1, y1, x2, y2,
			ZBuffer,SDL_MapRGB(surface->format, 0, 0, 0));
	}

	rastertriangles.clear();
	vertexnormbuffer.clear();
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
	tri.normal = triangleNormal;
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
	
	
	double t = (0.4 + (1 * DiffuseLightDir.getNormalized().getDotProduct(tri.normal.getNormalized()))) ;		//assime ambietn coefficnet 0.5 for r g and b
	if (t < 0.0) t = 0;
	if (t > 1.0) t = 1;
	tri.t = t;

}