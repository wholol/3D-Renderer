#include "FragmentShader.h"
#include "Draw.h"
#include "ScreenSize.h"
#include "PhongFlat_Frag.h"
#include "Gouraud_Frag.h"
#include "FlatShading_Frag.h"
#include "PhongPoint_Frag.h"

void FragmentShader::Process(SDL_Surface* surface, std::vector<Vector3f>& vertexnormbuffer,std::vector<triangle>& rastertriangles, Uint32 objcolor, std::shared_ptr<Light> light, bool wireframe, bool draw_normals)
{
	ZBuffer.reserve(SCREENHEIGHT * SCREENWIDTH);

	for (int i = 0; i < SCREENHEIGHT * SCREENWIDTH; ++i)
	{
		ZBuffer.emplace_back(INFINITY);
	}

	if (light->diffuse_type == Diffuse_Type::Gouraud_Shading)
	{
		//draw call for gouraud
		for (auto& t : rastertriangles)
		{
			Gouraud_Frag::filltriangle_gouraud(surface, t.points[0].x, t.points[0].y, t.points[0].z, t.points[1].x, t.points[1].y, t.points[1].z, t.points[2].x, t.points[2].y, t.points[2].z,
				ZBuffer, t.vertex_colors[0], t.vertex_colors[1], t.vertex_colors[2]);

			if (wireframe)
			{
				Draw::drawtriangle(surface, t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y);
			}
		}

		//visualize normals
		if (draw_normals)
		{
			for (auto& t : rastertriangles)
			{
				for (int i = 0; i <= 2; ++i)
				{
					Draw::drawline(surface, t.points[i].x, t.points[i].y, t.norm_end[i].x, t.norm_end[i].y, SDL_MapRGB(surface->format, 255, 0, 0));
				}
			}
		}
	}

	else if (light->diffuse_type == Diffuse_Type::Flat_Shading)
	{
		//draw call for flat shading
		for (auto& t : rastertriangles)
		{
			FlatShading_Frag::filltriangle_flat(surface, t.points[0].x, t.points[0].y, t.points[0].z, t.points[1].x, t.points[1].y, t.points[1].z, t.points[2].x, t.points[2].y, t.points[2].z, ZBuffer, t.color);

			if (wireframe)
			{
				Draw::drawtriangle(surface, t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y);
			}
		}

		if (draw_normals)
		{
			for (auto& t : rastertriangles)
			{
				for (int i = 0; i <= 2; ++i)
				{
					Draw::drawline(surface, t.points[i].x, t.points[i].y, t.norm_end[i].x, t.norm_end[i].y, SDL_MapRGB(surface->format, 255, 0, 0));
				}
			}
		}
	}

	else if (light->light_type == Light_Type::DirLight && light->diffuse_type == Diffuse_Type::Phong_Shading)
	{
		DirectionalLightSetup& dl = dynamic_cast<DirectionalLightSetup&>(*light);

		for (auto& t : rastertriangles)
		{
			PhongFlat_Frag::filltriangle_phong_flat(surface, t.points[0].x, t.points[0].y, t.points[0].z, t.points[1].x, t.points[1].y, t.points[1].z, t.points[2].x, t.points[2].y, t.points[2].z, ZBuffer, t.v_normal[0], t.v_normal[1],
				t.v_normal[2], dl.lightdir,objcolor);

			if (wireframe)
			{
				Draw::drawtriangle(surface, t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y);
			}
		}

		if (draw_normals)
		{
			for (auto& t : rastertriangles)
			{
				for (int i = 0; i <= 2; ++i)
				{
					Draw::drawline(surface, t.points[i].x, t.points[i].y, t.norm_end[i].x, t.norm_end[i].y, SDL_MapRGB(surface->format, 255, 0, 0));
				}
			}
		}

	}

	else if (light->light_type == Light_Type::PointLight && light->diffuse_type == Diffuse_Type::Phong_Shading)
	{
		PointLightSetup& pl = dynamic_cast<PointLightSetup&>(*light);

		for (auto& t : rastertriangles)
		{

			//PhongPoint_Frag::filltriangle_phong_point(surface, t.points[0].x, t.points[0].y, t.points[0].z, t.w[0], t.points[1].x, t.points[1].y, t.points[1].z, t.w[1], t.points[2].x, t.points[2].y, t.points[2].z,
				//t.w[2],
				//camerapos, ZBuffer, ProjMat,
				//t.v_normal[0], t.v_normal[1], t.v_normal[2], pl, objcolor);

			if (wireframe)
			{
				Draw::drawtriangle(surface, t.points[0].x, t.points[0].y, t.points[1].x, t.points[1].y, t.points[2].x, t.points[2].y);
			}
		}

		if (draw_normals)
		{
			for (auto& t : rastertriangles)
			{
				for (int i = 0; i <= 2; ++i)
				{
					Draw::drawline(surface, t.points[i].x, t.points[i].y, t.norm_end[i].x, t.norm_end[i].y, SDL_MapRGB(surface->format, 255, 0, 0));
				}
			}
		}
	}

	rastertriangles.clear();

	for (int i = 0; i < vertexnormbuffer.size(); ++i)
	{
		vertexnormbuffer[i] = { 0.0f ,0.0f ,0.0f };
	}

	ZBuffer.clear();
}
