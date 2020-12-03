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
		temp.worldpoints[0] = p1;
		temp.worldpoints[1] = p2;
		temp.worldpoints[2] = p3;
		temp.index[0] = a;	
		temp.index[1] = b;
		temp.index[2] = c;

		if (DontCullTriangle(temp, camerapos))			//if the triangle does not need to be culled, proceed
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
	

	for (int i = 0; i < rastertriangles.size(); ++i)
	{
		//extract rgba components of light colour
		Uint8 obj_col[4];
		
		obj_col[0] = (color & 0xFF000000) >> 24;
		obj_col[1] = (color & 0x00FF0000) >> 16;
		obj_col[2] = (color & 0x0000FF00) >> 8;
		obj_col[3] = (color & 0x000000FF);

	

		Uint8 diffuse_rgba[4];
		Uint8 p1_vertex_rgba[4];
		Uint8 p2_vertex_rgba[4];
		Uint8 p3_vertex_rgba[4];

		//use to access vertex normal buffer
		int p1_index = rastertriangles[i].index[0];
		int p2_index = rastertriangles[i].index[1];
		int p3_index = rastertriangles[i].index[2];
		
		//point light
		Vector3f to_light_p1 = PointLightPos - rastertriangles[i].worldpoints[0];
		double dist_p1 = to_light_p1.getMagnitude();
		float dist_p21 = to_light_p1.getMagnitude();
		
		Vector3f to_light_p2 = PointLightPos - rastertriangles[i].worldpoints[1];
		double dist_p2 = to_light_p2.getMagnitude();
		
		Vector3f to_light_p3 = PointLightPos - rastertriangles[i].worldpoints[2];
		double dist_p3 = to_light_p3.getMagnitude();
		
		to_light_p1.Normalize();
		to_light_p2.Normalize();
		to_light_p3.Normalize();
		
		//specular
		Vector3f ViewVector_p1 = camerapos - rastertriangles[i].worldpoints[0];
		Vector3f ViewVector_p2 = camerapos - rastertriangles[i].worldpoints[1];
		Vector3f ViewVector_p3 = camerapos - rastertriangles[i].worldpoints[2];
		Vector3f w_p1 = to_light_p1.getNormalized()* 2.0f *rastertriangles[i].normal.getNormalized().getDotProduct(to_light_p1);
		Vector3f w_p2 = to_light_p2.getNormalized() * 2.0f * rastertriangles[i].normal.getNormalized().getDotProduct(to_light_p2);
		Vector3f w_p3 = to_light_p3.getNormalized() * 2.0f * rastertriangles[i].normal.getNormalized().getDotProduct(to_light_p3);

		Vector3f R_p1 =  w_p1 - to_light_p1;
		Vector3f R_p2 =  w_p2 - to_light_p2;
		Vector3f R_p3 =  w_p3 - to_light_p3;
		
		const float shininess = 128.0f;
		
		float specular_p1 = std::max (0.0f , std::powf((ViewVector_p1.getNormalized().getDotProduct(R_p1.getNormalized())), shininess));
		if (specular_p1 > 1.0f)
		{
			specular_p1 = 1.0f;
		}
		float specular_p2 = std::max(0.0f, std::powf((ViewVector_p2.getNormalized().getDotProduct(R_p2.getNormalized())), shininess)  );
		if (specular_p2 > 1.0f)
		{
			specular_p2 = 1.0f;
		}
		float specular_p3 = std::max(0.0f, std::powf((ViewVector_p3.getNormalized().getDotProduct(R_p3.getNormalized())), shininess)  );
		if (specular_p3 > 1.0f)
		{
			specular_p3 = 1.0f;
		}

		this->specular_p1 = specular_p1;
		this->specular_p2 = specular_p2;
		this->specular_p3 = specular_p3;
		//attenuation factor
		//https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
		const double a = 0.01f;
		const double b = 0.5f;
		const double c = 0.382f;
		const double attenuation_p1 = 1.0 / ((a * dist_p1 * dist_p1) + (b * dist_p1) + 1);
		const double attenuation_p2 = 1.0 / ((a * dist_p2 * dist_p2) + (b * dist_p2) + 1);
		const double attenuation_p3 = 1.0 / ((a * dist_p3 * dist_p3) + (b * dist_p3) + 1);

		double p1_vertex_t = to_light_p1.getNormalized().getDotProduct(vertexnormbuffer[p1_index].getNormalized());
		if (p1_vertex_t < 0.0)
		{
			p1_vertex_t = 0.0;
		}

		double p2_vertex_t = to_light_p2.getNormalized().getDotProduct(vertexnormbuffer[p2_index].getNormalized());
		if (p2_vertex_t < 0.0)
		{
			p2_vertex_t = 0.0;
		}

		double p3_vertex_t = to_light_p3.getNormalized().getDotProduct(vertexnormbuffer[p3_index].getNormalized());
		if (p3_vertex_t < 0.0)
		{
			p3_vertex_t = 0.0;
		}

		p1_vertex_rgba[0] = obj_col[0];
		p2_vertex_rgba[0] = obj_col[0];
		p3_vertex_rgba[0] = obj_col[0];
		diffuse_rgba[0] = obj_col[0];

		for (int j = 1; j < 4; ++j)
		{
			diffuse_rgba[j] = obj_col[j] * (rastertriangles[i].t);		//indiviual diffuse colour output
			int c1 = obj_col[j] * (0.1 + attenuation_p1 * ( p1_vertex_t + specular_p1 * 20) );
			int c2 = obj_col[j] * (0.1 + attenuation_p2 * (  p2_vertex_t + specular_p2 * 20) );
			int c3 = obj_col[j] * (0.1 + attenuation_p3 * (  p3_vertex_t + specular_p3 * 20) );

			if (c1 > 255)
			{
				c1 = 255;
			}

			if (c2 > 255)
			{
				c2 = 255;
			}

			if (c3 > 255)
			{
				c3 = 255;
			}

			p1_vertex_rgba[j] = c1;		//indiviual diffuse colour output
			p2_vertex_rgba[j] = c2;		//indiviual diffuse colour output
			p3_vertex_rgba[j] = c3;		//indiviual diffuse colour output

			//rastertriangles[i].p1_rgba[j] = p1_vertex_rgba[j];
			//rastertriangles[i].p2_rgba[j] = p2_vertex_rgba[j];
			//rastertriangles[i].p3_rgba[j] = p3_vertex_rgba[j];
		}

		Uint32 diffuse_color, p1_color, p2_color, p3_color;
		diffuse_color = (diffuse_rgba[0] << 24) + (diffuse_rgba[1] << 16) + (diffuse_rgba[2] << 8) + diffuse_rgba[3];
		
		p1_color = (p1_vertex_rgba[0] << 24) + (p1_vertex_rgba[1] << 16) + (p1_vertex_rgba[2] << 8) + p1_vertex_rgba[3];
		p2_color = (p2_vertex_rgba[0] << 24) + (p2_vertex_rgba[1] << 16) + (p2_vertex_rgba[2] << 8) + p2_vertex_rgba[3];
		p3_color = (p3_vertex_rgba[0] << 24) + (p3_vertex_rgba[1] << 16) + (p3_vertex_rgba[2] << 8) + p3_vertex_rgba[3];
		
		rastertriangles[i].color = diffuse_color;//flat shading
		rastertriangles[i].p1_color = p1_color;
		rastertriangles[i].p2_color = p2_color;
		rastertriangles[i].p3_color = p3_color;
	}

	//pass params into draw
	for (int i = 0; i < rastertriangles.size(); ++i)
	{
		double p1_x = rastertriangles[i].points[0].x;
		double p2_x = rastertriangles[i].points[1].x;
		double p3_x = rastertriangles[i].points[2].x;

		double p1_y = rastertriangles[i].points[0].y;
		double p2_y = rastertriangles[i].points[1].y;
		double p3_y = rastertriangles[i].points[2].y;

		// plane equation parameters
		double d = (rastertriangles[i].points[0].getDotProduct(rastertriangles[i].normal.getNormalized()));
		double a_prime = rastertriangles[i].normal.x / d;
		double b_prime = rastertriangles[i].normal.y / d;
		double c_prime = rastertriangles[i].normal.z / d;
		double w = rastertriangles[i].w;

		//Draw::drawtriangle(surface, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, SDL_MapRGB(surface->format, 255, 255, 0));		//wrirefram colour to be black

		Draw::filltriangle(surface, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, vertexnormbuffer, rastertriangles[i].p1_color,
			rastertriangles[i].p2_color, rastertriangles[i].p3_color, color);

		int p1_index = rastertriangles[i].index[0];
		int p2_index = rastertriangles[i].index[1];
		int p3_index = rastertriangles[i].index[2];

		Vector3f normal_p1 = vertexnormbuffer[p1_index].getNormalized();
		Vector3f normal_p2 = vertexnormbuffer[p2_index].getNormalized();
		Vector3f normal_p3 = vertexnormbuffer[p3_index].getNormalized();

		//Draw::filltriangle_p(surface, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, vertexnormbuffer, normal_p1,
		//	normal_p2, normal_p3,color);

	}

	rastertriangles.clear();
	for (int i = 0; i < vertexnormbuffer.size(); ++i)
	{
		vertexnormbuffer[i] = { 0.0f ,0.0f ,0.0f };
	}

	ZBuffer.clear();
}

void Pipeline::testfunc()
{
	PointLightPos += {0.0f, 0.00f, 0.02f};
	std::cout << specular_p1 << " " << specular_p2 << " " << specular_p3 << std::endl;
}


void Pipeline::ModeltoWorldTransform(triangle& original,triangle& output)
{
	for (int i = 0; i < 3; ++i) {
		output.points[i] = ModelMat * original.points[i];
	}
}

bool Pipeline::DontCullTriangle(triangle& tri, Vector3f& camerapos)
{
	float n = tri.normal.getDotProduct(tri.points[0] - camerapos);
	
	if (n < 0.0f) {
		return true;
	}
	return false;
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
	
	
	double t =  DiffuseLightDir.getNormalized().getDotProduct(tri.normal.getNormalized()) ;		//assime ambietn coefficnet 0.5 for r g and b
	
	if (t < 0.0) t = 0;
	if (t > 1.0) t = 1;
	tri.t = t;

}