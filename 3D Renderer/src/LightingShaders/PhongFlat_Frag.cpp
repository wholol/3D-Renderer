#include "PhongFlat_Frag.h"
#include "src/Core/ScreenSize.h"

void PhongFlat_Frag::filltriangle_phong_flat(SDL_Surface * surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p3_x, double p3_y, double p3_z, std::vector<double>& ZBuffer, Vector3f v1_vertex, Vector3f v2_vertex, Vector3f v3_vertex, Vector3f lightdir,Uint32 color)
{
	if (p2_y < p1_y) {
		std::swap(p2_y, p1_y);
		std::swap(p2_x, p1_x);
		std::swap(v2_vertex, v1_vertex);
		std::swap(p2_z, p1_z);
	}

	if (p3_y < p1_y) {
		std::swap(p3_y, p1_y);
		std::swap(p3_x, p1_x);
		std::swap(v3_vertex, v1_vertex);
		std::swap(p3_z, p1_z);
	}

	if (p3_y < p2_y) {
		std::swap(p3_y, p2_y);
		std::swap(p3_x, p2_x);
		std::swap(v3_vertex, v2_vertex);
		std::swap(p3_z, p2_z);
	}

	if (p1_y == p2_y)
	{
		fillflattoptriangle_phong_flat(surface, p1_x, p1_y, p1_z, p2_x, p2_y, p2_z, p3_x, p3_y, p3_z, ZBuffer, v1_vertex, v2_vertex, v3_vertex, lightdir,color);
	}

	else if (p2_y == p3_y)
	{
		fillflatbottomtriangle_phong_flat(surface, p1_x, p1_y, p1_z, p2_x, p2_y, p2_z, p3_x, p3_y, p3_z, ZBuffer, v1_vertex, v2_vertex, v3_vertex, lightdir,color);
	}

	else {

		double x4 = (double)p1_x + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (double)(p3_x - p1_x);		//determine the oposite end of the triangle bottom/top
		double z4 = p1_z + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (double)(p3_z - p1_z);

		double t = (double)(p3_y - p2_y) / (double)(p3_y - p1_y);
		Vector3f v4_vertex = v3_vertex * (1.0 - t) + v1_vertex * t;
		v4_vertex.Normalize();


		//right side major by default perform a swap between x4 and p2_x in the functions if left side.
		//note that p2y is the middle, and it will be used for both sides).
		fillflatbottomtriangle_phong_flat(surface, p1_x, p1_y, p1_z, p2_x, p2_y, p2_z, x4, p3_y, z4, ZBuffer, v1_vertex, v2_vertex, v4_vertex, lightdir, color);
		fillflattoptriangle_phong_flat(surface, p2_x, p2_y, p2_z, x4, p1_y, z4, p3_x, p3_y, p3_z, ZBuffer, v4_vertex, v2_vertex, v3_vertex, lightdir, color);
	}
}

void PhongFlat_Frag::fillflatbottomtriangle_phong_flat(SDL_Surface * surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p4_x, double p3_y, double p4_z, std::vector<double>& ZBuffer, 
	Vector3f p1_vertex, Vector3f p2_vertex, Vector3f p4_vertex, Vector3f& lightdir, Uint32 color)
{
	//by default:
		//p1_x , p1_y = top of flat botom triangle
		//p2_x , p2_y = end of flat bottom triangle (vertex side)
		//p4_x , p2_y = end of flat bottom triangle (non vertex side)
		//p3_y = for end of overall triangle - used for interpolation 	

	if (p2_x > p4_x)	//if the triangle is left side major, i.e. non vertex side xpos < vertex side xpos
	{
		std::swap(p4_x, p2_x);
		std::swap(p4_z, p2_z);
		std::swap(p4_vertex, p2_vertex);
	}

	double dy = p2_y - p1_y;
	double dx1 = p2_x - p1_x;
	double dx2 = p4_x - p1_x;
	double dz1 = p2_z - p1_z;
	double dz2 = p4_z - p1_z;

	double slope_1 = 0; double slope_2 = 0;
	double slope_1_z = 0; double slope_2_z = 0;

	if (dy) {
		slope_1 = dx1 / dy;
		slope_1_z = dz1 / dy;
	}

	if (dy) {
		slope_2 = dx2 / dy;
		slope_2_z = dz2 / dy;
	}

	Vector3f Va, Vb, Vp;

	Uint8 light_src_rgba[4], final_light_rgba[4];
	Uint32 final_light;
	light_src_rgba[0] = (color & 0xFF000000) >> 24;
	light_src_rgba[1] = (color & 0x00FF0000) >> 16;
	light_src_rgba[2] = (color & 0x0000FF00) >> 8;
	light_src_rgba[3] = (color & 0x000000FF);
	final_light_rgba[0] = light_src_rgba[0];

	int yStart = (int)p1_y;
	int yEnd = (int)p2_y;

	if (yStart <= 1) { yStart = 1; }
	if (yEnd >= SCREENHEIGHT - 1) {
		yEnd = SCREENHEIGHT - 1;
	}

	for (int scanline = yStart; scanline <= yEnd; scanline++) {
		double px1 = slope_1 * ((double)scanline - p2_y) + p2_x;
		double px2 = slope_2 * ((double)scanline - p2_y) + p4_x;

		double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
		double zEnd = slope_2_z * ((double)scanline - p2_y) + p4_z;

		int xStart = (int)px1;
		int xEnd = (int)px2;

		if (xStart <= 1) { xStart = 1; }
		if (xEnd >= SCREENWIDTH) { xEnd = SCREENWIDTH; }

		double t = (double)(scanline - yEnd) / (double)(yStart - yEnd);

		//LHS
		Va = p1_vertex * t + p2_vertex * (1.0 - t);
		Va.Normalize();

		//RHS
		Vb = p1_vertex * t + p4_vertex * (1.0 - t);
		Vb.Normalize();

		for (int x = xStart; x <= xEnd; ++x) {

			double t_x = (double)(xEnd - x) / (double)(xEnd - xStart);

			double z_frag = zStart * t_x + zEnd * (1.0 - t_x);

			if (ZBuffer[x + SCREENWIDTH * scanline] > z_frag) {
				ZBuffer[x + SCREENWIDTH * scanline] = z_frag;

				if ((xStart - xEnd) == 0)	//if the point is at the begining
				{
					Vp(p1_vertex.x, p1_vertex.y, p1_vertex.z);
				}

				else
				{
					Vp = Va * t_x + Vb * (1.0 - t_x);
				}

				double diff_k = std::max(0.0f, Vp.Normalize().getDotProduct(lightdir.getNormalized()));
				double amb_k = 0.1;
				double f = diff_k + amb_k;

				for (int j = 1; j < 4; ++j)
				{
					int c = light_src_rgba[j] * f;
					if (c > 255) c = 255;
					final_light_rgba[j] = c;
				}

				final_light = (final_light_rgba[0] << 24) + (final_light_rgba[1] << 16) + (final_light_rgba[2] << 8) + final_light_rgba[3];
				Draw::putpixel(surface, x, scanline, final_light);
			}
		}
	}
}

void PhongFlat_Frag::fillflattoptriangle_phong_flat(SDL_Surface * surface, double p2_x, double p2_y, double p2_z, double p4_x, double p1_y, double p4_z, double p3_x, double p3_y, double p3_z, std::vector<double>& ZBuffer, 
	Vector3f p4_vertex, Vector3f p2_vertex, Vector3f p3_vertex, Vector3f lightdir, Uint32 color)
{
	//by default:
		//p2_x , p2_y = top left of flat top triangle (middle vertex coordinate)
		//p4_x , p2_y = end right flat top triangle 
		//p3_x , p3_y = end of flat top triangle 
		//p1_y = for top of overall triangle - used for goroud interpolation of colour intensity 

	if (p2_x > p4_x)
	{
		std::swap(p2_x, p4_x);
		std::swap(p2_z, p4_z);
		std::swap(p4_vertex, p2_vertex);
	}

	double dy = p3_y - p2_y;
	double dx1 = p3_x - p2_x;
	double dx2 = p3_x - p4_x;
	double dz1 = p3_z - p2_z;
	double dz2 = p3_z - p4_z;

	double slope_1 = 0; double slope_2 = 0;
	double slope_1_z = 0; double slope_2_z = 0;

	if (dy) {
		slope_1 = dx1 / dy;
		slope_1_z = dz1 / dy;
	}

	if (dy) {
		slope_2 = dx2 / dy;
		slope_2_z = dz2 / dy;
	}

	if (p2_y <= 0) { p2_y = 0; }
	if (p3_y >= SCREENHEIGHT - 1) { p3_y = SCREENHEIGHT - 1; }

	Vector3f Va, Vb, Vp;

	Uint8 light_src_rgba[4], final_light_rgba[4];
	Uint32 final_light;
	light_src_rgba[0] = (color & 0xFF000000) >> 24;
	light_src_rgba[1] = (color & 0x00FF0000) >> 16;
	light_src_rgba[2] = (color & 0x0000FF00) >> 8;
	light_src_rgba[3] = (color & 0x000000FF);
	final_light_rgba[0] = light_src_rgba[0];

	int yStart = (int)p3_y;
	int yEnd = (int)p2_y;

	if (yStart >= SCREENHEIGHT -1 ) { yStart = SCREENHEIGHT - 1; }
	if (yEnd <= 1) { yEnd = 1; }

	for (int scanline = yStart; scanline > yEnd; scanline--) {
		double px1 = slope_1 * ((double)scanline - p2_y) + p2_x;
		double px2 = slope_2 * ((double)scanline - p3_y) + p3_x;

		double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
		double zEnd = slope_2_z * ((double)scanline - p3_y) + p3_z;

		int xStart = (int)px1;
		int xEnd = (int)px2;

		if (xStart <= 1) { xStart = 1; }
		if (xEnd >= SCREENWIDTH - 1) { xEnd = SCREENWIDTH - 1; }

		double t = (double)(scanline - yEnd) / (double)(yStart - yEnd);

		//LHS
		Va = p3_vertex * t + p2_vertex * (1.0 - t);
		Va.Normalize();

		//RHS
		Vb = p3_vertex * t + p4_vertex * (1.0 - t);
		Vb.Normalize();

		for (int x = xStart; x <= xEnd; ++x) {

			float t_x = (double)(xEnd - x) / (double)(xEnd - xStart);
			double z_frag = zStart * t_x + zEnd * (1.0 - t_x);

			if (ZBuffer[x + SCREENWIDTH * scanline] > z_frag) {
				ZBuffer[x + SCREENWIDTH * scanline] = z_frag;

				if ((xStart - xEnd) == 0)	//if the point is at the vertex
				{
					Vp(p3_vertex.x, p3_vertex.y, p3_vertex.z);
				}

				else
				{
					Vp = Va * t_x + Vb * (1.0 - t_x);
				}

				//clacualte colour intensity with vp
				//directional light
				double diff_k = std::max(0.0f, Vp.Normalize().getDotProduct(lightdir.getNormalized()));
				double amb_k = 0.1;

				double f = diff_k + amb_k;

				for (int j = 1; j < 4; ++j)
				{
					int c = (int)(light_src_rgba[j] * f);
					if (c > 255) c = 255;
					final_light_rgba[j] = c;
				}

				final_light = (final_light_rgba[0] << 24) + (final_light_rgba[1] << 16) + (final_light_rgba[2] << 8) + final_light_rgba[3];
				Draw::putpixel(surface, x, scanline, final_light);
			}
		}
	}
}
