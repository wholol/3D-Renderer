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

void Pipeline::ComputeLighting(std::shared_ptr<Light>& light, std::vector<triangle>& tris)
{
	Uint8 obj_col[4];
	obj_col[0] = (light->light_col & 0xFF000000) >> 24;
	obj_col[1] = (light->light_col & 0x00FF0000) >> 16;
	obj_col[2] = (light->light_col & 0x0000FF00) >> 8;
	obj_col[3] = (light->light_col & 0x000000FF);

	Uint8 temp[4];	//temporary to modify colors
	temp[0] = obj_col[0];
	temp[1] = obj_col[1];
	temp[2] = obj_col[2];
	temp[3] = obj_col[3];

	if (light->light_type == Light_Type::PointLight)
	{
		PointLightSetup& pl = dynamic_cast<PointLightSetup&>(*light);

		if (pl.diffuse_type == Diffuse_Type::Gouraud_Shading || pl.diffuse_type == Diffuse_Type::Flat_Shading) {
			for (auto& t : tris) {
				for (int i = 0; i < 3; ++i)		//for each vertex
				{
					//ambient
					double amb_k = pl.amb_constant;
					//attenuation
					Vector3f to_light = pl.lightpos - t.worldpoints[i];
					double dist = to_light.getMagnitude();	//get distance from point lgiht to vertex point
					double attenuation = 1.0 / ((pl.a * dist * dist) + (pl.b * dist) + pl.c);	//get attenuation
					to_light.Normalize();	//normalize
					//diffuse
					double diff_k;
					if (pl.diffuse_type == Diffuse_Type::Gouraud_Shading)
					{
						//gouraud
						diff_k = std::max(0.0f , to_light.getNormalized().getDotProduct(t.v_normal[i].getNormalized()));
					}
					else 
					{
						//flat shading
						diff_k = std::max(0.0f, to_light.getNormalized().getDotProduct(t.s_normal.getNormalized()));
					}
					
					//specular
					Vector3f ViewVec = camerapos - t.worldpoints[i];
					Vector3f w = to_light * 2.0 * t.v_normal[i].getNormalized().getDotProduct(to_light);
					Vector3f r = w - to_light;
					double spec_k = std::max(0.0f, std::powf((ViewVec.getNormalized().getDotProduct(r.getNormalized())), pl.spec_exponent));

					if (spec_k > 1.0f)
					{
						spec_k = 1.0f;
					}

					double f = (amb_k + attenuation * (diff_k + (spec_k * pl.spec_intensity)));
					
					for (int j = 1; j < 4; ++j)
					{
						int c = obj_col[j] * f;
						if (c > 255) c = 255;
						temp[j] = c;
					}

					if (pl.diffuse_type == Diffuse_Type::Gouraud_Shading) {
						t.vertex_colors[i] = (temp[0] << 24) + (temp[1] << 16) + (temp[2] << 8) + temp[3];
					}
					else {
						//flat shading
						t.color = (temp[0] << 24) + (temp[1] << 16) + (temp[2] << 8) + temp[3];
					}
				}
			}
		}	
	}

	if (light->light_type == Light_Type::DirLight)
	{
		DirectionalLightSetup& dl = dynamic_cast<DirectionalLightSetup&>(*light);

		if (dl.diffuse_type == Diffuse_Type::Gouraud_Shading || dl.diffuse_type == Diffuse_Type::Flat_Shading) {
			for (auto& t : tris) {
				for (int i = 0; i < 3; ++i)		//for each vertex
				{
					//ambient
					double amb_k = dl.amb_constant;

					//diffuse
					double diff_k;
					if (dl.diffuse_type == Diffuse_Type::Gouraud_Shading)
					{
						//gouraud
						diff_k = std::max(0.0f, DiffuseLightDir.getNormalized().getDotProduct(t.v_normal[i].getNormalized()));
					}

					else {
						//flat shading
						diff_k = std::max(0.0f, DiffuseLightDir.getNormalized().getDotProduct(t.s_normal.getNormalized()));
					}

					double f = amb_k  + diff_k;

					for (int j = 1; j < 4; ++j)
					{
						int c = obj_col[j] * f;
						if (c > 255) c = 255;

						temp[j] = c;
					}

					if (dl.diffuse_type == Diffuse_Type::Gouraud_Shading) {
						t.vertex_colors[i] = (temp[0] << 24) + (temp[1] << 16) + (temp[2] << 8) + temp[3];
					}
					else {
						//flat shading
						t.color = (temp[0] << 24) + (temp[1] << 16) + (temp[2] << 8) + temp[3];
					}
				}
			}
		}
	}
}

void Pipeline::setCamera(Vector3f& camerapos, Vector3f& lookDir)
{
	ViewMat = Mat3f::CameraPointAt(camerapos, lookDir);
	ViewMat = Mat3f::InverseCamera(ViewMat);
	this->camerapos = camerapos;
}

void Pipeline::setupTriangles(mesh& m, std::vector<int>& indexbuffer, std::vector<Vector3f>& vertexbuffer, std::vector<Vector3f>& vertexnormbuffer,bool testforcull)
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
	}

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

		//setup triangle params (geotemtry shader)
		triangle temp;
		Vector3f line1 = p3 - p1;
		Vector3f line2 = p2 - p1;
		temp.s_normal = line2.getCrossProduct(line1).getNormalized();
		temp.points[0] = p1;
		temp.points[1] = p2;
		temp.points[2] = p3;
		temp.v_normal[0] = vertexnormbuffer[a].Normalize();
		temp.v_normal[1] = vertexnormbuffer[b].Normalize();
		temp.v_normal[2] = vertexnormbuffer[c].Normalize();
		
		if (DontCullTriangle(temp, camerapos))			//if the triangle does not need to be culled, proceed
		{
			WorldtoCameraTransform(temp);	
			temp.worldpoints[0] = temp.points[0];
			temp.worldpoints[1] = temp.points[1];
			temp.worldpoints[2] = temp.points[2];
			//TODO:Z plane clipping here
			triangle new_tris[2];
			int num_tris = trianglestoclip(Vector3f{ 0.0f , 0.0f , 1.0f }, Vector3f{ 0.0f , 0.0f , 1.0f }, temp, new_tris[0], new_tris[1]);	//clip agaisnt z axis
			for (int i = 0; i < num_tris; ++i)
			{
				NDCTransform(new_tris[i]);
			
				ViewPortTransform(new_tris[i]);
		
				rastertriangles.emplace_back(new_tris[i]);
			}
		}
	}
}

void Pipeline::Draw(SDL_Surface* surface, std::vector<Vector3f>& vertexnormbuffer, Uint32 color, std::shared_ptr<Light> light, bool wireframe)
{
	ComputeLighting(light, rastertriangles);	//compute the lighting for rastertriangles in geometry shader
	if (light->diffuse_type == Diffuse_Type::Gouraud_Shading)
	{
		//draw call for gouraud
		for (auto& t : rastertriangles)
		{
			double d = (t.points[0].getDotProduct(t.s_normal.getNormalized()));
			double a_prime = t.s_normal.x / d;
			double b_prime = t.s_normal.y / d;
			double c_prime = t.s_normal.z / d;
			double w = t.w;
			Draw::drawtriangle(surface, t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y,
				w, a_prime, b_prime, c_prime, d, ZBuffer);
			Draw::filltriangle_gouraud(surface,t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y, 
				 w, a_prime, b_prime, c_prime, d, ZBuffer, t.vertex_colors[0] , t.vertex_colors[1] , t.vertex_colors[2]);
			
		}
	}

	else if (light->diffuse_type == Diffuse_Type::Flat_Shading)
	{
		//draw call for flat shading
		for (auto& t : rastertriangles)
		{
			double d = (t.points[0].getDotProduct(t.s_normal.getNormalized()));
			double a_prime = t.s_normal.x / d;
			double b_prime = t.s_normal.y / d;
			double c_prime = t.s_normal.z / d;
			double w = t.w;
			Draw::drawtriangle(surface, t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y,
				w, a_prime, b_prime, c_prime, d, ZBuffer);
			Draw::filltriangle_flat(surface, t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y,
				w, a_prime, b_prime, c_prime, d, ZBuffer, t.color);
		}
	}

	else if (light->light_type == Light_Type::DirLight && light->diffuse_type == Diffuse_Type::Phong_Shading)
	{
		DirectionalLightSetup& dl = dynamic_cast<DirectionalLightSetup&>(*light);
		for (auto& t : rastertriangles)
		{
			double d = (t.points[0].getDotProduct(t.s_normal.getNormalized()));
			double a_prime = t.s_normal.x / d;
			double b_prime = t.s_normal.y / d;
			double c_prime = t.s_normal.z / d;
			double w = t.w;
			Draw::drawtriangle(surface, t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y,
				w, a_prime, b_prime, c_prime, d, ZBuffer);
			Draw::filltriangle_phong_flat(surface, t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y, w, a_prime, b_prime, c_prime, d, ZBuffer, vertexnormbuffer, t.v_normal[0], t.v_normal[1], 
				t.v_normal[2] , dl.lightdir , color);
		}
	}

	else if (light->light_type == Light_Type::PointLight && light->diffuse_type == Diffuse_Type::Phong_Shading)
	{
		PointLightSetup& pl = dynamic_cast<PointLightSetup&>(*light);
		for (auto& t : rastertriangles)
		{
			double d = (t.points[0].getDotProduct(t.s_normal.getNormalized()));
			double a_prime = t.s_normal.x / d;
			double b_prime = t.s_normal.y / d;
			double c_prime = t.s_normal.z / d;
			double w = t.w;
			//draw call
		}
		
	}

	rastertriangles.clear();
	for (int i = 0; i < vertexnormbuffer.size(); ++i)
	{
		vertexnormbuffer[i] = { 0.0f ,0.0f ,0.0f };
	}

	ZBuffer.clear();
}

void Pipeline::testfunc(std::shared_ptr<PointLightSetup> light)
{
	
		PointLightSetup& pl = dynamic_cast<PointLightSetup&>(*light);
		pl.lightpos += { 0, 0, 0.5};
	
}


void Pipeline::ModeltoWorldTransform(triangle& original,triangle& output)
{
	for (int i = 0; i < 3; ++i) {
		output.points[i] = ModelMat * original.points[i];
	}
}

bool Pipeline::DontCullTriangle(triangle& tri, Vector3f& camerapos)
{
	float n = tri.s_normal.getDotProduct(tri.points[0] - camerapos);
	
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

void Pipeline::ViewPortTransform(triangle& tri)
{
	for (int i = 0; i < 3; ++i)
	{
		tri.points[i] = Mat3f::Translate(1, 1, 0) * tri.points[i];

		tri.points[i] = Mat3f::Scale(0.5 * ScreenWidth, 0.5 * ScreenHeight, 1) * tri.points[i];
	}
}

void Pipeline::clip(triangle& tri)
{
	
}

Vector3f Pipeline::intersectPlane(Vector3f& plane, Vector3f& plane_normal, Vector3f& lineStart, Vector3f& lineEnd)
{
	//UC DAVIS lectuer on clipping
	plane_normal.Normalize();

	float d1 = plane_normal.getDotProduct(lineStart - plane);
	float d2 = plane_normal.getDotProduct(lineEnd - plane);
	float t = d1 / (d1 - d2);
	Vector3f I = lineStart + (lineEnd - lineStart) * t;

	//float plane_d = plane_normal.getDotProduct(plane);
	//float ad = lineStart.getDotProduct(plane_normal);
	//float bd = lineEnd.getDotProduct(plane_normal);
	//float t = (-plane_d - ad) / (bd - ad);
	//Vector3f lineStartToEnd = lineEnd - lineStart;
	//Vector3f linetoIntersect = lineStartToEnd * t;
	//return (lineStart + linetoIntersect);
	return I;
}

int Pipeline::trianglestoclip(Vector3f plane, Vector3f plane_normal, triangle& tri, triangle& new1, triangle& new2)
{
	plane_normal.Normalize();

	auto dist = [&](Vector3f& p)
	{
		return (p - plane).getDotProduct(plane_normal);
	};

	//compute distance of each point
	float p0_dist = dist(tri.points[0]);
	float p1_dist = dist(tri.points[1]);
	float p2_dist = dist(tri.points[2]);

	 int nInsidePointCount = 0;
     int nOutsidePointCount = 0;
	
	 if (p0_dist <= 0) { nOutsidePointCount++; }
	 else { nInsidePointCount++;}
	 if (p1_dist <= 0){ nOutsidePointCount++; }
	 else{ nInsidePointCount++; }
	 if (p2_dist <= 0) { nOutsidePointCount++; }
	 else { nInsidePointCount++; }

	if (nInsidePointCount == 0)
	{
		return 0;	//whole triangle is culled.
	}

	else if (nInsidePointCount == 3)	//all points inside, no need to clip
	{
		new1 = tri;
		return 1;
	}

	else if (nInsidePointCount == 1 && nOutsidePointCount == 2)	//one point inside, two out. form ONE new triangle
	{
		if (p0_dist >= 0)	//if point[0] inside
		{
			new1.points[0] = tri.points[0];
			new1.points[1] = intersectPlane(plane, plane_normal, tri.points[0], tri.points[1]);
			new1.points[2] = intersectPlane(plane, plane_normal, tri.points[0], tri.points[2]);
			new1.worldpoints[0] = tri.worldpoints[0];
			new1.worldpoints[1] = intersectPlane(plane, plane_normal, tri.worldpoints[0], tri.worldpoints[1]);
			new1.worldpoints[2] = intersectPlane(plane, plane_normal, tri.worldpoints[0], tri.worldpoints[2]);
			new1.s_normal = tri.s_normal;
			new1.v_normal[0] = tri.v_normal[0];
			new1.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
			new1.v_normal[2] = tri.s_normal;
		}

		else if (p1_dist >= 0)	//if point[1] inside
		{
			new1.points[0] = intersectPlane(plane, plane_normal, tri.points[1], tri.points[0]);
			new1.points[1] = tri.points[1];
			new1.points[2] = intersectPlane(plane, plane_normal, tri.points[1], tri.points[2]);
			new1.worldpoints[0] = intersectPlane(plane, plane_normal, tri.worldpoints[1], tri.worldpoints[0]);
			new1.worldpoints[1] = tri.worldpoints[1];
			new1.worldpoints[2] = intersectPlane(plane, plane_normal, tri.worldpoints[1], tri.worldpoints[2]);
			new1.s_normal = tri.s_normal;
			new1.v_normal[0] = tri.s_normal;
			new1.v_normal[1] = tri.v_normal[1];	//since the point is at the edge of the sreen, v.normal = s.normal.
			new1.v_normal[2] = tri.s_normal;
		}

		else if (p2_dist >= 0)	//if point[2] is inside
		{
			new1.points[0] = intersectPlane(plane, plane_normal, tri.points[2], tri.points[0]);
			new1.points[1] = intersectPlane(plane, plane_normal, tri.points[2], tri.points[1]);
			new1.points[2] = tri.points[2];
			new1.worldpoints[0] = intersectPlane(plane, plane_normal, tri.worldpoints[2], tri.worldpoints[0]);
			new1.worldpoints[1] = intersectPlane(plane, plane_normal, tri.worldpoints[2], tri.worldpoints[1]);
			new1.worldpoints[2] = tri.worldpoints[2];
			new1.s_normal = tri.s_normal;
			new1.v_normal[0] = tri.s_normal;
			new1.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
			new1.v_normal[2] = tri.v_normal[2];
		}
		return 1;
	}

	//WEIRD ISSUE HERE
	else if (nInsidePointCount == 2 && nOutsidePointCount == 1)	//two point inside, one out. form TWO new triangle
	{
		//02 01 12
		if (p0_dist >= 0 && p1_dist >=0)	//if point[0] and point[1] inside ,p[2] is OUT
		{
			//connects with a new point and p1
			new1.points[0] = tri.points[0];
			new1.points[1] = intersectPlane(plane, plane_normal, tri.points[0], tri.points[2]);
			new1.points[2] = tri.points[1];
			new1.worldpoints[0] = tri.worldpoints[0];
			new1.worldpoints[1] = intersectPlane(plane, plane_normal, tri.worldpoints[0], tri.worldpoints[2]);
			new1.worldpoints[2] = tri.worldpoints[1];
			new1.s_normal = tri.s_normal;
			new1.v_normal[0] = tri.v_normal[0];
			new1.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
			new1.v_normal[2] = tri.v_normal[1];

			//conects with both new points
			new2.points[0] = tri.points[1];
			new2.points[1] = new1.points[1];
			new2.points[2] = intersectPlane(plane, plane_normal, tri.points[1], tri.points[2]);
			new2.worldpoints[0] = tri.worldpoints[1];
			new2.worldpoints[1] = new1.worldpoints[1];
			new2.worldpoints[2] = intersectPlane(plane, plane_normal, tri.worldpoints[1], tri.worldpoints[2]);
			new2.s_normal = tri.s_normal;
			new2.v_normal[0] = tri.v_normal[1];
			new2.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
			new2.v_normal[2] = tri.s_normal;
		}

		else if (p1_dist >= 0 && p2_dist >=0)	//if point 1 and 2 is inside
		{
			//connects with a new point and p1
			new1.points[0] = tri.points[1];
			new1.points[1] = intersectPlane(plane, plane_normal, tri.points[1], tri.points[0]);
			new1.points[2] = tri.points[2];
			new1.worldpoints[0] = tri.worldpoints[1];
			new1.worldpoints[1] = intersectPlane(plane, plane_normal, tri.worldpoints[1], tri.worldpoints[0]);
			new1.worldpoints[2] = tri.worldpoints[2];
			new1.s_normal = tri.s_normal;
			new1.v_normal[0] = tri.v_normal[1];
			new1.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
			new1.v_normal[2] = tri.v_normal[2];

			//conects with both new points
			new2.points[0] = tri.points[2];
			new2.points[1] = new1.points[1];
			new2.points[2] = intersectPlane(plane, plane_normal, tri.points[2], tri.points[0]);
			new2.worldpoints[0] = tri.worldpoints[1];
			new2.worldpoints[1] = new1.worldpoints[1];
			new2.worldpoints[2] = intersectPlane(plane, plane_normal, tri.worldpoints[2], tri.worldpoints[0]);
			new2.s_normal = tri.s_normal;
			new2.v_normal[0] = tri.v_normal[2];
			new2.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
			new2.v_normal[2] = tri.s_normal;
		}

		else if (p0_dist >= 0 && p2_dist >= 0)	//if point 0 and 2 is inside
		{
			//connects with a new point and p1
			new1.points[0] = tri.points[0];
			new1.points[1] = intersectPlane(plane, plane_normal, tri.points[0], tri.points[1]);
			new1.points[2] = tri.points[2];
			new1.worldpoints[0] = tri.worldpoints[0];
			new1.worldpoints[1] = intersectPlane(plane, plane_normal, tri.worldpoints[0], tri.worldpoints[1]);
			new1.worldpoints[2] = tri.worldpoints[2];
			new1.s_normal = tri.s_normal;
			new1.v_normal[0] = tri.v_normal[0];
			new1.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
			new1.v_normal[2] = tri.v_normal[2];

			//conects with both new points
			new2.points[0] = tri.points[2];
			new2.points[1] = new1.points[1];
			new2.points[2] = intersectPlane(plane, plane_normal, tri.points[2], tri.points[1]);
			new2.worldpoints[0] = tri.worldpoints[2];
			new2.worldpoints[1] = new1.worldpoints[1];
			new2.worldpoints[2] = intersectPlane(plane, plane_normal, tri.worldpoints[2], tri.worldpoints[1]);
			new2.s_normal = tri.s_normal;
			new2.v_normal[0] = tri.v_normal[2];
			new2.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
			new2.v_normal[2] = tri.s_normal;
		}

		return 2;
	}

}
