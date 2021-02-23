#include "VertexShader.h"

void VertexShader::setProjectionMatrix(float FovDegrees, float Near, float Far, unsigned int ScreenHeight, unsigned int ScreenWidth)
{
	float AspectRatio = ((float)ScreenWidth / (float)ScreenHeight);
	this->ScreenHeight = ScreenHeight;
	this->ScreenWidth = ScreenWidth;
	ProjMat = Mat3f::Projection(Near, Far, AspectRatio, FovDegrees);
}

void VertexShader::setTransformMatrix(const Mat3f finalTransform)
{
	ModelMat = finalTransform;
}

void VertexShader::setViewMatrix(Vector3f & camerapos, Vector3f & lookDir)
{
	ViewMat = Mat3f::CameraPointAt(camerapos, lookDir);
	ViewMat = Mat3f::InverseCamera(ViewMat);
	this->camerapos = camerapos;
}

void VertexShader::ProcessPrimitive(std::vector<int>& indexbuffer, std::vector<Vector3f>& vertexbuffer, std::vector<Vector3f>& vertexnormbuffer,std::shared_ptr<Light> light, bool testforcull)
{
	for (int i = 0u; i < indexbuffer.size(); i += 3)
	{
		int a = indexbuffer[i];
		int b = indexbuffer[i + 1];
		int c = indexbuffer[i + 2];

		Vector3f p1 = vertexbuffer[a];
		Vector3f p2 = vertexbuffer[b];
		Vector3f p3 = vertexbuffer[c];

		//model to world transform (vertex normals are used for lighting computation in view space).
		p1 = ModelMat *  p1;
		p2 = ModelMat *  p2;
		p3 = ModelMat *  p3;

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

		temp.points[0] = p1;
		temp.points[1] = p2;
		temp.points[2] = p3;

		//store world positions (for lighting computation)
		temp.worldpoints[0] = p1;
		temp.worldpoints[1] = p2;
		temp.worldpoints[2] = p3;

		Vector3f line1 = p3 - p1;
		Vector3f line2 = p2 - p1;
		temp.s_normal = line2.getCrossProduct(line1).getNormalized();

		temp.v_normal[0] = vertexnormbuffer[a].Normalize();
		temp.v_normal[1] = vertexnormbuffer[b].Normalize();
		temp.v_normal[2] = vertexnormbuffer[c].Normalize();

		if (DontCullTriangle(temp, camerapos))			//if the triangle does not need to be culled, proceed
		{
			temp.norm_end[0] = temp.points[0] + temp.v_normal[0] * 0.5;
			temp.norm_end[1] = temp.points[1] + temp.v_normal[1] * 0.5;
			temp.norm_end[2] = temp.points[2] + temp.v_normal[2] * 0.5;

			WorldtoCameraTransform(temp);
	
			temp.viewpoints[0] = ViewMat * temp.points[0];
			temp.viewpoints[1] = ViewMat * temp.points[1];
			temp.viewpoints[2] = ViewMat * temp.points[2];

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

	ComputeLighting(light);	//computes the lighting at the vertices
}

std::vector<triangle>& VertexShader::getRasterTriangles()
{
	return rastertriangles;
}
