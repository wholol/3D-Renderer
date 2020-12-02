#pragma once
#include "Mat.h"
#include <SDL.h>
#include "mesh.h"

class Pipeline {
	//the pipeline = load model -> (vertex shader) transform -> cull -> camera -> proejct -> viewport -> clipping ->pixel shading
public:
	
	void setProjectionParams(float FovDegrees, float Near, float Far,unsigned int ScreenHeight, unsigned int ScreenWidth);	//projection amtrix params

	void setTransformations(const Mat3f finalTransform = Mat3f::Identity());		//set the transformation matrix

	void setDiffuseLight(triangle& tri);

	void setCamera(Vector3f& camerapos,Vector3f& lookDir);

	void Render( mesh& m, std::vector<int>& indexbuffer, std::vector<Vector3f>& vertexbuffer, std::vector<Vector3f>& vertexnormbuffer ,bool testforcull = true);	//should transform coordinates to space, and perform scan line conversion.

	void Draw(SDL_Surface* surface, std::vector<Vector3f>& vertexnormbuffer,  Uint32 color,bool wireframe = true);

	void testfunc();

	double specular_p1, specular_p2, specular_p3;
	
private:

	Mat3f MVPMat;			//MVP matrix
	Mat3f ProjMat;
	Mat3f ViewMat;
	Mat3f ModelMat;
	Vector3f camerapos;
	unsigned int ScreenHeight, ScreenWidth;
	std::vector<triangle> rastertriangles;
	
	std::vector<float> ZBuffer;
	

	/*lighting*/
	Vector3f DiffuseLightDir = { 0.0f , 0.0f , -1.0f};	//to be set be user 
	Vector3f PointLightPos = { 0.0f , 0.0f , 0.0f };

	//functions to be called in rednering pipeline
	void ModeltoWorldTransform(triangle& original,triangle& output);
	bool DontCullTriangle(triangle& tri,Vector3f& camerapos);
	void WorldtoCameraTransform(triangle& tri);
	void NDCTransform(triangle& tri);
	void sortZDirection();
	void ViewPortTransform(triangle& tri);
	


};