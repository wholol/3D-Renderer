#pragma once
#include "Mat.h"
#include <SDL.h>
#include "mesh.h"
#include "LightSetup.h"
#include "Light_Type.h"

class Pipeline {
	//the pipeline = load model -> (vertex shader) transform -> cull -> camera -> proejct -> viewport -> clipping ->pixel shading
public:
	void setProjectionParams(float FovDegrees, float Near, float Far,unsigned int ScreenHeight, unsigned int ScreenWidth);	//projection amtrix params

	void setTransformations(const Mat3f finalTransform = Mat3f::Identity());		//set the transformation matrix

	void setLightColor(const Uint32& lightcol);

	void setCamera(Vector3f& camerapos,Vector3f& lookDir);

	void setupTriangles( mesh& m, std::vector<int>& indexbuffer, std::vector<Vector3f>& vertexbuffer, std::vector<Vector3f>& vertexnormbuffer ,bool testforcull = true);	//should transform coordinates to space, and perform scan line conversion.

	void Draw(SDL_Surface* surface, std::vector<Vector3f>& vertexnormbuffer, Uint32 color, std::shared_ptr<Light> light, bool wireframe = true);

	void testfunc();
	
private:
	Mat3f MVPMat;			
	Mat3f ProjMat;
	Mat3f ViewMat;
	Mat3f ModelMat;
	Vector3f camerapos;
	unsigned int ScreenHeight, ScreenWidth;
	std::vector<triangle> rastertriangles;
	std::vector<double> ZBuffer;
	
	/*lighting*/
	Vector3f DiffuseLightDir = { 0.0f , 0.0f , -1.0f};	//to be set be user 
	Vector3f PointLightPos = { 0.0f , 0.0f , 0.0f };
	Uint32 lightcol;

	//functions to be called in rednering pipeline
	void ModeltoWorldTransform(triangle& original,triangle& output);
	bool DontCullTriangle(triangle& tri,Vector3f& camerapos);
	void WorldtoCameraTransform(triangle& tri);
	void NDCTransform(triangle& tri);
	void sortZDirection();
	void ViewPortTransform(triangle& tri);
	//lighting functions
	void ComputeLighting(std::shared_ptr<Light> light, std::vector<triangle>& tris);		//compute the lgihting for gouraud or flat shading
	void setDiffuseLight(triangle& tri);
	void setAmbientLight(double t);
	//void setSpecularLight(triangle& tri);

};