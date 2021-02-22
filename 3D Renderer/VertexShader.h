#pragma once
#include "Mat.h"
#include <vector>
#include "triangle.h"
#include "LightSetup.h"
#include <algorithm>
#include "SDL.h"
#include <iostream>

class VertexShader
{
public:

	void setProjectionMatrix(float FovDegrees, float Near, float Far, unsigned int ScreenHeight, unsigned int ScreenWidth);
	void setTransformMatrix(const Mat3f finalTransform = Mat3f::Identity());
	void setViewMatrix(Vector3f& camerapos, Vector3f& lookDir);
	void ProcessPrimitive(std::vector<int>& indexbuffer, std::vector<Vector3f>& vertexbuffer, std::vector<Vector3f>& vertexnormbuffer, std::shared_ptr<Light> light, bool testforcull = true);
	std::vector<triangle>& getRasterTriangles();
	
public:
	Mat3f ProjMat;	//needed for phong shading, hence public
	Mat3f ViewMat; //needed for phong shading, hence public
private:
	Mat3f MVPMat;
	
	Mat3f ModelMat;
	std::vector<triangle> rastertriangles;
	int ScreenWidth, ScreenHeight;
	Vector3f camerapos;

	//HELPER FUNCTIONS AND IMPLEMENTATIONS BELOW

	void ModeltoWorldTransform(triangle& original, triangle& output)
	{
		for (int i = 0; i < 3; ++i) {
			output.points[i] = ModelMat * original.points[i];
		}
	}
	
	bool DontCullTriangle(triangle& tri, Vector3f& camerapos)
	{
		float n = tri.s_normal.getDotProduct(tri.points[0] - camerapos);

		if (n < 0.0f) {
			return true;
		}
		return false;
	}
	
	void WorldtoCameraTransform(triangle& tri)
	{
		for (int i = 0; i < 3; ++i) {
			tri.points[i] = ViewMat * tri.points[i];
			tri.norm_end[i] = ViewMat * tri.norm_end[i];
		}
	}

	void NDCTransform(triangle& tri)
	{
		for (int i = 0; i < 3; ++i) {
			tri.points[i] = ProjMat * tri.points[i];
			tri.points[i] = Mat3f::Normalize(tri.points[i], ProjMat);
			tri.w[i] = 1.0 / (ProjMat.w);				//store the w value of the projection matrix
			tri.ww = ProjMat.w;

			tri.norm_end[i] = ProjMat * tri.norm_end[i];
			tri.norm_end[i] = Mat3f::Normalize(tri.norm_end[i], ProjMat);

		}
	}


	void ViewPortTransform(triangle& tri)
	{
		for (int i = 0; i < 3; ++i)
		{
			tri.points[i] = Mat3f::Translate(1, 1, 0) * tri.points[i];

			tri.points[i] = Mat3f::Scale(0.5 * ScreenWidth, 0.5 * ScreenHeight, 1) * tri.points[i];

			tri.norm_end[i] = Mat3f::Translate(1, 1, 0) * tri.norm_end[i];

			tri.norm_end[i] = Mat3f::Scale(0.5 * ScreenWidth, 0.5 * ScreenHeight, 1) * tri.norm_end[i];
		}
	}


	//Cyrus-Beck Line Clipping algorithm
	Vector3f intersectPlane(Vector3f& plane, Vector3f& plane_normal, Vector3f& lineStart, Vector3f& lineEnd)
	{
		//UC DAVIS lectuer on clipping
		plane_normal.Normalize();

		float d1 = plane_normal.getDotProduct(lineStart - plane);
		float d2 = plane_normal.getDotProduct(lineEnd - plane);
		float t = d1 / (d1 - d2);
		Vector3f I = lineStart + (lineEnd - lineStart) * t;


		return I;
	}


	int trianglestoclip(Vector3f plane, Vector3f plane_normal, triangle& tri, triangle& new1, triangle& new2)
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
		else { nInsidePointCount++; }
		if (p1_dist <= 0) { nOutsidePointCount++; }
		else { nInsidePointCount++; }
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
				new1.viewpoints[0] = tri.viewpoints[0];
				new1.viewpoints[1] = intersectPlane(plane, plane_normal, tri.viewpoints[0], tri.viewpoints[1]);
				new1.viewpoints[2] = intersectPlane(plane, plane_normal, tri.viewpoints[0], tri.viewpoints[2]);
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
				new1.viewpoints[0] = intersectPlane(plane, plane_normal, tri.viewpoints[1], tri.viewpoints[0]);
				new1.viewpoints[1] = tri.viewpoints[1];
				new1.viewpoints[2] = intersectPlane(plane, plane_normal, tri.viewpoints[1], tri.viewpoints[2]);
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
				new1.viewpoints[0] = intersectPlane(plane, plane_normal, tri.viewpoints[2], tri.viewpoints[0]);
				new1.viewpoints[1] = intersectPlane(plane, plane_normal, tri.viewpoints[2], tri.viewpoints[1]);
				new1.viewpoints[2] = tri.viewpoints[2];
				new1.s_normal = tri.s_normal;
				new1.v_normal[0] = tri.s_normal;
				new1.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
				new1.v_normal[2] = tri.v_normal[2];
			}
			return 1;
		}

		else if (nInsidePointCount == 2 && nOutsidePointCount == 1)	//two point inside, one out. form TWO new triangle
		{
			//02 01 12
			if (p0_dist >= 0 && p1_dist >= 0)	//if point[0] and point[1] inside ,p[2] is OUT
			{
				//connects with a new point and p1
				new1.points[0] = tri.points[0];
				new1.points[1] = intersectPlane(plane, plane_normal, tri.points[0], tri.points[2]);
				new1.points[2] = tri.points[1];
				new1.viewpoints[0] = tri.viewpoints[0];
				new1.viewpoints[1] = intersectPlane(plane, plane_normal, tri.viewpoints[0], tri.viewpoints[2]);
				new1.viewpoints[2] = tri.viewpoints[1];
				new1.s_normal = tri.s_normal;
				new1.v_normal[0] = tri.v_normal[0];
				new1.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
				new1.v_normal[2] = tri.v_normal[1];

				//conects with both new points
				new2.points[0] = tri.points[1];
				new2.points[1] = new1.points[1];
				new2.points[2] = intersectPlane(plane, plane_normal, tri.points[1], tri.points[2]);
				new2.viewpoints[0] = tri.viewpoints[1];
				new2.viewpoints[1] = new1.viewpoints[1];
				new2.viewpoints[2] = intersectPlane(plane, plane_normal, tri.viewpoints[1], tri.viewpoints[2]);
				new2.s_normal = tri.s_normal;
				new2.v_normal[0] = tri.v_normal[1];
				new2.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
				new2.v_normal[2] = tri.s_normal;
			}

			else if (p1_dist >= 0 && p2_dist >= 0)	//if point 1 and 2 is inside
			{
				//connects with a new point and p1
				new1.points[0] = tri.points[1];
				new1.points[1] = intersectPlane(plane, plane_normal, tri.points[1], tri.points[0]);
				new1.points[2] = tri.points[2];
				new1.viewpoints[0] = tri.viewpoints[1];
				new1.viewpoints[1] = intersectPlane(plane, plane_normal, tri.viewpoints[1], tri.viewpoints[0]);
				new1.viewpoints[2] = tri.viewpoints[2];
				new1.s_normal = tri.s_normal;
				new1.v_normal[0] = tri.v_normal[1];
				new1.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
				new1.v_normal[2] = tri.v_normal[2];

				//conects with both new points
				new2.points[0] = tri.points[2];
				new2.points[1] = new1.points[1];
				new2.points[2] = intersectPlane(plane, plane_normal, tri.points[2], tri.points[0]);
				new2.viewpoints[0] = tri.viewpoints[1];
				new2.viewpoints[1] = new1.viewpoints[1];
				new2.viewpoints[2] = intersectPlane(plane, plane_normal, tri.viewpoints[2], tri.viewpoints[0]);
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
				new1.viewpoints[0] = tri.viewpoints[0];
				new1.viewpoints[1] = intersectPlane(plane, plane_normal, tri.viewpoints[0], tri.viewpoints[1]);
				new1.viewpoints[2] = tri.viewpoints[2];
				new1.s_normal = tri.s_normal;
				new1.v_normal[0] = tri.v_normal[0];
				new1.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
				new1.v_normal[2] = tri.v_normal[2];

				//conects with both new points
				new2.points[0] = tri.points[2];
				new2.points[1] = new1.points[1];
				new2.points[2] = intersectPlane(plane, plane_normal, tri.points[2], tri.points[1]);
				new2.viewpoints[0] = tri.viewpoints[2];
				new2.viewpoints[1] = new1.viewpoints[1];
				new2.viewpoints[2] = intersectPlane(plane, plane_normal, tri.viewpoints[2], tri.viewpoints[1]);
				new2.s_normal = tri.s_normal;
				new2.v_normal[0] = tri.v_normal[2];
				new2.v_normal[1] = tri.s_normal;	//since the point is at the edge of the sreen, v.normal = s.normal.
				new2.v_normal[2] = tri.s_normal;
			}

			return 2;
		}
	}

	//lighting functions
	void ComputeLighting(std::shared_ptr<Light>& light)
	{

		Uint8 obj_col[4];
		obj_col[0] = (light->light_col & 0xFF000000) >> 24;
		obj_col[1] = (light->light_col & 0x00FF0000) >> 16;
		obj_col[2] = (light->light_col & 0x0000FF00) >> 8;
		obj_col[3] = (light->light_col & 0x000000FF);

		Uint8 temp[4];	//temporary to modify colors
		temp[0] = obj_col[0];

		if (light->light_type == Light_Type::PointLight)
		{
			PointLightSetup& pl = dynamic_cast<PointLightSetup&>(*light);

			if (pl.diffuse_type == Diffuse_Type::Gouraud_Shading || pl.diffuse_type == Diffuse_Type::Flat_Shading) {
				
				for (auto& t : rastertriangles) {
					
					for (int i = 0; i < 3; ++i)		//for each vertex of each triangle
					{
						//ambient
						double amb_k = pl.amb_constant;
						
						//attenuation
						Vector3f to_light =  pl.lightpos - t.worldpoints[i];
					
						double dist = to_light.getMagnitude();	//get distance from point lgiht to vertex point
						double attenuation = 1.0 / ((pl.a * dist * dist) + (pl.b * dist) + pl.c);	//get attenuation
						to_light.Normalize();
						
						//diffuse
						double diff_k;
						
						if (pl.diffuse_type == Diffuse_Type::Gouraud_Shading)
						{
							//gouraud
							diff_k = std::max( 0.0f, to_light.getDotProduct(t.v_normal[i]));
						}

						else
						{
							//flat shading
							diff_k = std::max(0.0f, to_light.getDotProduct(t.s_normal.getNormalized()));
						}

						//specular
						Vector3f ViewVec =  camerapos - t.worldpoints[i];
						Vector3f w = t.v_normal[i] * 2.0 * t.v_normal[i].getDotProduct(to_light);
						Vector3f r = w - to_light;

						//( R . V ) ^ shininess
						double spec_k = std::max(0.0f, std::powf((ViewVec.getNormalized().getDotProduct(r.getNormalized())), pl.spec_exponent));

						if (spec_k > 1.0)
						{
							spec_k = 1.0;
						}
						
						double f = (amb_k + attenuation * (diff_k +(spec_k * pl.spec_intensity)));

						for (int j = 1; j < 4; ++j)	//compute rgb for each vertex
						{
							int c = obj_col[j] * f;
							if (c >= 255) c = 255;
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
				for (auto& t : rastertriangles) {
					for (int i = 0; i < 3; ++i)		//for each vertex
					{
						Uint8 temp[4];	//temporary to modify colors
						temp[0] = obj_col[0];
						//ambient
						double amb_k = dl.amb_constant;

						//diffuse
						double diff_k;
						if (dl.diffuse_type == Diffuse_Type::Gouraud_Shading)
						{
							//gouraud

							diff_k = std::max(0.0f, dl.lightdir.getNormalized().getDotProduct(t.v_normal[i]));
						}

						else {
							//flat shading
							diff_k = std::max(0.0f, dl.lightdir.getNormalized().getDotProduct(t.s_normal.getNormalized()));
						}

						double f = amb_k + diff_k;

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

};