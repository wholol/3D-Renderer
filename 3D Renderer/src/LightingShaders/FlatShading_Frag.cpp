#include "FlatShading_Frag.h"
#include "src/Core/ScreenSize.h"

void FlatShading_Frag::filltriangle_flat(SDL_Surface * surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p3_x, double p3_y, double p3_z, std::vector<double>& ZBuffer, Uint32 color)
{
	if (p2_y < p1_y) {
		std::swap(p2_y, p1_y);
		std::swap(p2_x, p1_x);
		std::swap(p2_z, p1_z);
	}

	if (p3_y < p1_y) {
		std::swap(p3_y, p1_y);
		std::swap(p3_x, p1_x);
		std::swap(p3_z, p1_z);
	}

	if (p3_y < p2_y) {
		std::swap(p3_y, p2_y);
		std::swap(p3_x, p2_x);
		std::swap(p3_z, p2_z);
	}

	if (p1_y == p2_y)
	{
		fillflattoptriangle_flat(surface, p1_x, p1_y, p1_z, p2_x, p2_y, p2_z, p3_x, p3_y, p3_z, ZBuffer, color);
	}

	else if (p2_y == p3_y)
	{
		fillflatbottomtriangle_flat(surface, p1_x, p1_y, p1_z, p2_x, p2_y, p2_z, p3_x, p3_y, p3_z, ZBuffer, color);
	}

	else {

		double x4 = p1_x + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (p3_x - p1_x);		//determine the oposite end of the triangle bottom/top
		double z4 = p1_z + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (double)(p3_z - p1_z);

		fillflatbottomtriangle_flat(surface, p1_x, p1_y, p1_z, p2_x, p2_y, p2_z, x4, p3_y, z4, ZBuffer, color);
		fillflattoptriangle_flat(surface, p2_x, p2_y, p2_z, x4, p1_y, z4, p3_x, p3_y, p3_z, ZBuffer, color);

	}
}

void FlatShading_Frag::fillflatbottomtriangle_flat(SDL_Surface * surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p4_x, double p3_y, double p4_z, std::vector<double>& ZBuffer, Uint32 color)
{
	//by default:
		//p1_x , p1_y = top of flat botom triangle
		//p2_x , p2_y = end of flat bottom triangle (vertex side)
		//p4_x , p2_y = end of flat bottom triangle (non vertex side)
		//p3_y = for end of overall triangle - used for goroud interpolation of colour intensity 

	if (p2_x > p4_x)	//if the triangle is left side major, i.e. non vertex side xpos < vertex side xpos
	{
		std::swap(p4_x, p2_x);
		std::swap(p4_z, p2_z);
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

	if (p1_y <= 1) { p1_y = 1; }
	if (p2_y >= SCREENHEIGHT - 1) { p2_y = SCREENHEIGHT - 1; }

	for (int scanline = p1_y; scanline <= p2_y; scanline++) {
		double px1 = slope_1 * ((double)scanline - p2_y) + p2_x;
		double px2 = slope_2 * ((double)scanline - p2_y) + p4_x;

		double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
		double zEnd = slope_2_z * ((double)scanline - p2_y) + p4_z;

		int xStart = (int)px1;
		int xEnd = (int)px2;

		if (xStart <= 1) { xStart = 1; }
		if (xEnd >= 800) { xEnd = 799; }

		for (int x = xStart; x <= xEnd; ++x) {

			double t = (double)(xEnd - x) / (double)(xEnd - xStart);
			double z_frag = zStart * t + zEnd * (1.0 - t);

			if (ZBuffer[x + SCREENWIDTH * scanline] > z_frag) {
				ZBuffer[x + SCREENWIDTH * scanline] = z_frag;
					Draw::putpixel(surface, x, scanline, color);
			}
		}
	}
}

void FlatShading_Frag::fillflattoptriangle_flat(SDL_Surface * surface, double p2_x, double p2_y, double p2_z, double p4_x, double p1_y, double p4_z, double p3_x, double p3_y, double p3_z, std::vector<double>& ZBuffer, Uint32 color)
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

	if (p2_y <= 1) { p2_y = 1; }
	if (p3_y >= 599) { p3_y = 599; }

	for (int scanline = p3_y; scanline >= p2_y; scanline--) {
		double px1 = slope_1 * ((double)scanline - p2_y) + p2_x;
		double px2 = slope_2 * ((double)scanline - p3_y) + p3_x;

		double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
		double zEnd = slope_2_z * ((double)scanline - p3_y) + p3_z;

		int xStart = (int)px1;
		int xEnd = (int)px2;

		if (xStart <= 1) { xStart = 1; }
		if (xEnd >= SCREENWIDTH - 1 ) { xEnd = SCREENWIDTH - 1; }

		for (int x = xStart; x <= xEnd; ++x) {

			double t_x = (double)(xEnd - x) / (double)(xEnd - xStart);
			double z_frag = zStart * t_x + zEnd * (1.0 - t_x);

			if (ZBuffer[x + SCREENWIDTH * scanline] > z_frag) {
				ZBuffer[x + SCREENWIDTH * scanline] = z_frag;
					Draw::putpixel(surface, x, scanline, color);
			}
		}
	}
}
