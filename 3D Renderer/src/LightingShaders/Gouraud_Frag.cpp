#include "Gouraud_Frag.h"
#include "src/Core/ScreenSize.h"

void Gouraud_Frag::filltriangle_gouraud(SDL_Surface * surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p3_x, double p3_y, double p3_z, std::vector<double>& ZBuffer, Uint32 p1_color, Uint32 p2_color, Uint32 p3_color)
{
	if (p2_y < p1_y) {
		std::swap(p2_y, p1_y);
		std::swap(p2_x, p1_x);
		std::swap(p2_color, p1_color);
		std::swap(p2_z, p1_z);
	}

	if (p3_y < p1_y) {
		std::swap(p3_y, p1_y);
		std::swap(p3_x, p1_x);
		std::swap(p3_color, p1_color);
		std::swap(p3_z, p1_z);
	}

	if (p3_y < p2_y) {
		std::swap(p3_y, p2_y);
		std::swap(p3_x, p2_x);
		std::swap(p3_color, p2_color);
		std::swap(p3_z, p2_z);
	}

	if (p1_y == p2_y)
	{
		fillflattoptriangle_gouraud(surface, p1_x, p1_y, p1_z, p2_x, p2_y, p2_z, p3_x, p3_y, p3_z, ZBuffer, p1_color, p2_color, p3_color);
	}

	else if (p2_y == p3_y)
	{
		fillflatbottomtriangle_gouraud(surface, p1_x, p1_y, p1_z, p2_x, p2_y, p2_z, p3_x, p3_y, p3_z, ZBuffer, p1_color, p2_color, p3_color);
	}

	else {
		//lerp end point opposite of vertex
		double x4 = p1_x + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (p3_x - p1_x);		//determine the oposite end of the triangle bottom/top
		double t = (double)(p3_y - p2_y) / (double)(p3_y - p1_y);

		double z4 = p1_z + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (double)(p3_z - p1_z);
		//lerp lightcolor
		Uint8 p4_color_rgba[4];
		Uint32 p4_color;
		p4_color_rgba[0] = (((p3_color & 0x00FF0000) >> 24) * (1 - t)) + (((p1_color & 0x00FF0000) >> 24) * t);
		p4_color_rgba[1] = (((p3_color & 0x00FF0000) >> 16) * (1 - t)) + (((p1_color & 0x00FF0000) >> 16) * t);
		p4_color_rgba[2] = (((p3_color & 0x0000FF00) >> 8) * (1 - t)) + (((p1_color & 0x0000FF00) >> 8) * t);
		p4_color_rgba[3] = (((p3_color & 0x000000FF)) * (1 - t)) + (((p1_color & 0x000000FF)) * t);
		p4_color = (p4_color_rgba[0] << 24) + (p4_color_rgba[1] << 16) + (p4_color_rgba[2] << 8) + (p4_color_rgba[3]);
		//right side major by default perform a swap between x4 and p2_x in the functions if left side.
		//note that p2y is the middle, and it will be used for both sides).

		fillflatbottomtriangle_gouraud(surface, p1_x, p1_y, p1_z, p2_x, p2_y, p2_z, x4, p3_y, z4, ZBuffer, p1_color, p2_color, p4_color);
		fillflattoptriangle_gouraud(surface, p2_x, p2_y, p2_z, x4, p1_y, z4, p3_x, p3_y, p3_z, ZBuffer, p4_color, p2_color, p3_color);
	}
}

void Gouraud_Frag::fillflatbottomtriangle_gouraud(SDL_Surface * surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p4_x, double p3_y, double p4_z, std::vector<double>& ZBuffer, Uint32 p1_color, Uint32 p2_color, Uint32 p3_color)
{
	//by default:
		//p1_x , p1_y = top of flat botom triangle
		//p2_x , p2_y = end of flat bottom triangle (vertex side)
		//p4_x , p2_y = end of flat bottom triangle (non vertex side)
		//p3_y = for end of overall triangle - used for goroud interpolation of colour intensity 

	bool left_side_major = false;

	if (p2_x > p4_x)	//if the triangle is left side major, i.e. non vertex side xpos < vertex side xpos
	{
		left_side_major = true;
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

	double Ip[4], Ib[4], Ia[4];
	
	int yStart = p1_y;		
	int yEnd = p2_y;

	if (yStart <= 1) { yStart = 1; }
	if (yEnd >= SCREENHEIGHT - 1) { yEnd = SCREENHEIGHT - 1; }

	for (int scanline = yStart; scanline <= yEnd; scanline++) {

		double px1 = slope_1 * ((double)scanline - p2_y) + p2_x;
		double px2 = slope_2 * ((double)scanline - p2_y) + p4_x;

		double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
		double zEnd = slope_2_z * ((double)scanline - p2_y) + p4_z;

		int xStart = (int)px1;
		int xEnd = (int)px2;

		if (xStart <= 1) xStart = 1;
		if (xEnd >= SCREENWIDTH - 1) xEnd = SCREENWIDTH - 1;

		//goroud calculation : https://www.youtube.com/watch?v=06p86OrTGLc&t=233s&ab_channel=raviramamoorthi
		//vertex side intensity
		//Ia = (p3_vertex * (scanline - p2_y) + v2_vertex * (p1_y - scanline)) / (p1_y - p2_y);
		double t = ((double)scanline - p2_y) / (p1_y - p2_y);

		Ia[1] = ((p1_color & 0x00FF0000) >> 16) * t + ((p2_color & 0x00FF0000) >> 16) * (1.0 - t);
		Ia[2] = ((p1_color & 0x0000FF00) >> 8) * t + ((p2_color & 0x0000FF00) >> 8) * (1.0 - t);
		Ia[3] = (p1_color & 0x000000FF) * t + (p2_color & 0x000000FF) *  (1.0 - t);

		//Ib = (p3_vertex * (scanline - p3_y) + v3_vertex * (p1_y - scanline)) / (p1_y - p3_y);
		Ib[1] = ((p1_color & 0x00FF0000) >> 16) *  t + ((p3_color & 0x00FF0000) >> 16) * (1.0 - t);
		Ib[2] = ((p1_color & 0x0000FF00) >> 8) *  t + ((p3_color & 0x0000FF00) >> 8) *  (1.0 - t);
		Ib[3] = (p1_color & 0x000000FF) *  t + (p3_color & 0x000000FF) *  (1.0 - t);

		if (left_side_major)
		{
			std::swap(Ia, Ib);
		}

		for (int x = xStart; x <= xEnd; ++x) {

			double t_x = (double)(xEnd - x) / (double)(xEnd - xStart);
			double z_frag = zStart * t_x + zEnd * (1.0 - t_x);

			if (ZBuffer[x + SCREENWIDTH * scanline] > z_frag) {
				ZBuffer[x + SCREENWIDTH * scanline] = z_frag;
				if ((xStart - xEnd) == 0)	//if the point is at the vertex
				{
					Ip[0] = (p1_color & 0xFF000000) >> 24;
					Ip[1] = (p1_color & 0x00FF0000) >> 16;
					Ip[2] = (p1_color & 0x0000FF00) >> 8;
					Ip[3] = (p1_color & 0x000000FF);
				}

				else
				{
					for (int i = 1; i < 4; ++i)
					{
						Ip[i] = (Ia[i] * t_x) + (Ib[i] * (1.0 - t_x));
					}
				}
			
				Draw::putpixel(surface, x, scanline, ((Uint8)Ip[0] << 24) + ((Uint8)Ip[1] << 16) + ((Uint8)Ip[2] << 8) + (Uint8)Ip[3]);
			}
		}
	}
}

void Gouraud_Frag::fillflattoptriangle_gouraud(SDL_Surface * surface, double p2_x, double p2_y, double p2_z, double p4_x, double p1_y, double p4_z, double p3_x, double p3_y, double p3_z, std::vector<double>& ZBuffer, Uint32 p1_color, Uint32 p2_color, Uint32 p3_color)
{
	//by default:
		//p2_x , p2_y = top left of flat top triangle (middle vertex coordinate)
		//p4_x , p2_y = end right flat top triangle 
		//p3_x , p3_y = end of flat top triangle 
		//p1_y = for top of overall triangle - used for goroud interpolation of colour intensity 

	bool left_side_major = false;
	if (p2_x > p4_x)
	{
		left_side_major = true;
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

	double Ip[4], Ib[4], Ia[4];
	
	int yStart = (int)p3_y;
	int yEnd = (int)p2_y;

	if (yStart >= SCREENHEIGHT - 1) { yStart = SCREENHEIGHT - 1; }
	if (yEnd <= 1) { yEnd = 1; }


	for (int scanline = yStart; scanline > yEnd; scanline--) {
		double px1 = slope_1 * ((double)scanline - p2_y) + p2_x;
		double px2 = slope_2 * ((double)scanline - p3_y) + p3_x;

		double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
		double zEnd = slope_2_z * ((double)scanline - p3_y) + p3_z;

		int xStart = (int)px1;
		int xEnd = (int)px2;
		if (xStart <= 1) xStart = 1;
		if (xEnd >= SCREENWIDTH - 1) xEnd = SCREENWIDTH - 1;

		double t = ((double)scanline - p2_y) / (p3_y - p2_y);

		Ia[1] = ((p3_color & 0x00FF0000) >> 16) * t + ((p2_color & 0x00FF0000) >> 16) * (1.0 - t);
		Ia[2] = ((p3_color & 0x0000FF00) >> 8) * t + ((p2_color & 0x0000FF00) >> 8) * (1.0 - t);
		Ia[3] = (p3_color & 0x000000FF) * t + (p2_color & 0x000000FF) * (1.0 - t);

		//Ib = (p3_vertex * (scanline - p3_y) + v3_vertex * (p1_y - scanline)) / (p1_y - p3_y);
		Ib[1] = ((p3_color & 0x00FF0000) >> 16) *  t + ((p1_color & 0x00FF0000) >> 16) * (1.0 - t);
		Ib[2] = ((p3_color & 0x0000FF00) >> 8) *  t + ((p1_color & 0x0000FF00) >> 8) *  (1.0 - t);
		Ib[3] = (p3_color & 0x000000FF) *  t + (p1_color & 0x000000FF) * (1.0 - t);

		if (left_side_major)
		{
			std::swap(Ia, Ib);
		}

		for (int x = xStart; x <= xEnd; ++x) {

			double t_x = (double)(xEnd - x) / (double)(xEnd - xStart);
			double z_frag = zStart * t_x + zEnd * (1.0 - t_x);

			if (ZBuffer[x + SCREENWIDTH * scanline] > z_frag) {
				ZBuffer[x + SCREENWIDTH * scanline] = z_frag;

				if ((xStart - xEnd) == 0)	//if the point is at the vertex
				{
					Ip[0] = (p1_color & 0xFF000000) >> 24;
					Ip[1] = (p1_color & 0x00FF0000) >> 16;
					Ip[2] = (p1_color & 0x0000FF00) >> 8;
					Ip[3] = (p1_color & 0x000000FF);
				}

				else
				{
					for (int i = 1; i < 4; ++i)
					{
						Ip[i] = (Ia[i] * t_x) + (Ib[i] * (1.0 - t_x));
					}
				}
				Draw::putpixel(surface, x, scanline, ((Uint8)Ip[0] << 24) + ((Uint8)Ip[1] << 16) + ((Uint8)Ip[2] << 8) + (Uint8)Ip[3]);
			}
		}
	}
}
