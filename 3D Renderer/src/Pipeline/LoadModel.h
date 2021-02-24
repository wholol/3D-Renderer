#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "triangle.h"
#include "tiny_obj_loader.h"
#include <iostream>

class LoadModel
{
public:
	
	std::vector<Vector3f> vertexbuffer;
	std::vector<Vector3f> vertexnormbuffer;
	std::vector<int> indexbuffer;
	std::string path = "src/Models/";
	
public:
	void loadFromFile(const std::string& objfile);
};