#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "mesh.h"
#include "tiny_obj_loader.h"
#include <iostream>

class LoadModel
{
public:
	void loadFromFile(const std::string& filepath, mesh& m) {
	
		std::ifstream f(filepath);
		
		if (!f)
		{
			throw std::runtime_error("file opening failed.");
		}

		// Local cache of verts
		std::vector<Vector3f> verts;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::stringstream s;
			s << line;

			char junk;

			if (line[0] == 'v')	//load vertex
			{
				Vector3<float> v;
				v.setComponents(s);
				verts.emplace_back(v);
			}

			if (line[0] == 'f')		//load faces
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				m.triangles.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });		
			}
		}
	}

	void loadFromFile(std::vector<triangle>& points, mesh& m)
	{
		m.triangles = std::move(points);
			
	}

	
	std::vector<Vector3f> vertexbuffer;
	std::vector<Vector3f> vertexnormbuffer;
	std::vector<int> indexbuffer;
	
	void loadFromFile(const std::string& objfile)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;

		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objfile.c_str());

		if (!warn.empty()) {
			std::cout << warn << std::endl;
		}

		if (!err.empty()) {
			std::cerr << err << std::endl;
		}

		if (!ret) {
			exit(1);
		}

		//load vertex buffer
		for (int i = 0; i < attrib.vertices.size(); i += 3)
		{
			vertexbuffer.push_back({ attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2] });
		}

		//initialize vertex nromal buffers
		for (int i = 0; i < vertexbuffer.size(); ++i)
		{
			vertexnormbuffer.push_back({ 0 , 0 , 0 });
		}

		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

				int fv = shapes[s].mesh.num_face_vertices[f];		//number of vertices per face

				if (fv != 3u)
				{
					throw std::runtime_error(" face is not a triangle.");
				}

				// Loop over vertices in the face. since its triagle, loop three times.
				for (size_t v = 0; v < fv; v++) {
					//access to index
					tinyobj::index_t idx = shapes[s].mesh.indices[f * 3u + v];
					indexbuffer.push_back(idx.vertex_index);
				}
			}
		}


	}
};