#pragma once
#include "SDL.h"
//FRAGMENT PART
#include <algorithm>
#include <iostream>
class Draw {

public:

	static void putpixel(SDL_Surface *surface, int x, int y,  Uint32 pixel = 0xFFFFFF)
	{
			int bpp = surface->format->BytesPerPixel;
			/* Here p is the address to the pixel we want to set */
			Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

			switch (bpp) {
			case 1:
				*p = pixel;
				break;

			case 2:
				*(Uint16 *)p = pixel;
				break;

			case 3:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
					p[0] = (pixel >> 16) & 0xff;
					p[1] = (pixel >> 8) & 0xff;
					p[2] = pixel & 0xff;
				}
				else {
					p[0] = pixel & 0xff;
					p[1] = (pixel >> 8) & 0xff;
					p[2] = (pixel >> 16) & 0xff;
				}
				break;

			case 4:
				*(Uint32 *)p = pixel;
				break;
			}
	}
	
	//phong fill
	static void filltriangle_p(SDL_Surface* surface, double p1_x, double p1_y, double p2_x, double p2_y, double p3_x, double p3_y, double w, double a_prime, double b_prime, double c_prime,
		double d, std::vector<double>& ZBuffer, std::vector<Vector3f>& vertexnormbuffer, Vector3f v1_vertex, Vector3f v2_vertex, Vector3f v3_vertex, Uint32 color = 0xFFFFFF)
	{
		if (p2_y < p1_y) {
			std::swap(p2_y, p1_y);
			std::swap(p2_x, p1_x);
			std::swap(v2_vertex, v1_vertex);
		}

		if (p3_y < p1_y) {
			std::swap(p3_y, p1_y);
			std::swap(p3_x, p1_x);
			std::swap(v3_vertex, v1_vertex);
		}

		if (p3_y < p2_y) {
			std::swap(p3_y, p2_y);
			std::swap(p3_x, p2_x);
			std::swap(v3_vertex, v2_vertex);
		}

		double x4 = p1_x + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (p3_x - p1_x);		//determine the oposite end of the triangle bottom/top

		//right side major by default perform a swap between x4 and p2_x in the functions if left side.
		//note that p2y is the middle, and it will be used for both sides).
		fillflattoptriangle_p(surface, p2_x, p2_y, x4, p1_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, vertexnormbuffer, v1_vertex, v2_vertex, v3_vertex, color);
		fillflatbottomtriangle_p(surface, p1_x, p1_y, p2_x, p2_y, x4, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, vertexnormbuffer, v1_vertex, v2_vertex, v3_vertex, color);
	}


	static void filltriangle_gouraud(SDL_Surface* surface, double p1_x, double p1_y, double p2_x, double p2_y, double p3_x, double p3_y,float w, double a_prime, double b_prime, double c_prime,
		double d , std::vector<double>& ZBuffer,Uint32 p1_color, Uint32 p2_color, Uint32 p3_color)
	{
		if (p2_y < p1_y) {		
			std::swap(p2_y, p1_y);
			std::swap(p2_x, p1_x);
			std::swap(p2_color, p1_color);
		}

		if (p3_y < p1_y) {
			std::swap(p3_y, p1_y);
			std::swap(p3_x, p1_x);
			std::swap(p3_color, p1_color);
		}

		if (p3_y < p2_y) {
			std::swap(p3_y, p2_y);
			std::swap(p3_x, p2_x);
			std::swap(p3_color, p2_color);
		}

		double x4 = p1_x + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (p3_x - p1_x);		//determine the oposite end of the triangle bottom/top
			
		//right side major by default perform a swap between x4 and p2_x in the functions if left side.
		//note that p2y is the middle, and it will be used for both sides).
		fillflatbottomtriangle_gouraud(surface, p1_x, p1_y, p2_x, p2_y, x4, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, p1_color, p2_color, p3_color);
		fillflattoptriangle_gouraud(surface, p2_x, p2_y, x4, p1_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, p1_color, p2_color, p3_color);
	}

	static void filltriangle_flat(SDL_Surface* surface, double p1_x, double p1_y, double p2_x, double p2_y, double p3_x, double p3_y, float w, double a_prime, double b_prime, double c_prime,
		double d, std::vector<double>& ZBuffer, Uint32 color)
	{
		if (p2_y < p1_y) {
			std::swap(p2_y, p1_y);
			std::swap(p2_x, p1_x);
		}

		if (p3_y < p1_y) {
			std::swap(p3_y, p1_y);
			std::swap(p3_x, p1_x);
		}

		if (p3_y < p2_y) {
			std::swap(p3_y, p2_y);
			std::swap(p3_x, p2_x);
		}

		double x4 = p1_x + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (p3_x - p1_x);		//determine the oposite end of the triangle bottom/top

		fillflatbottomtriangle_flat(surface, p1_x, p1_y, p2_x, p2_y, x4, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, color);
		fillflattoptriangle_flat(surface, p2_x, p2_y, x4, p1_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, color);
	}

	static void filltriangleTex(SDL_Surface* surface, SDL_Texture *texture, int u0, int v0, int u1, int v1,int u2,int v2, std::vector<double>& ZBuffer, Uint32 color = 0xFFFFFF)
	{
		if (v1 < v0) {
			std::swap(u1, u0);
			std::swap(v1, v0);
		}

		if (v2 < v0) {
			std::swap(u2, u0);
			std::swap(v2, v0);
		}

		if (v2 < v1) {
			std::swap(u2, u1);
			std::swap(v2, v1);
		}

		/*if it is only a bottom flat triangle*/
		if (v1 == v2)
		{
			if (u1 > u2) {
				std::swap(u1, u2);
			}
			//fillbottomtriangle(surface, u0, v0, u1, v1, u2, v2, ZBuffer, color);
		}

		else if (v0 == v1) {
			if (u0 > u1) {
				std::swap(u0, u1);
			}
			//filltoptriangle(surface, u0, v0, u1, v1, u2, v2, ZBuffer, color);
		}

		else {
			int u4 = u0 + (float)((float)(v1 - v0) / (float)(v2 - v0)) * (u2 - u0);
			if (u4 > u1) {
				//fillbottomtriangle(surface, u0, v0, u1, v1, u4, v1,  ZBuffer, color);
				//filltoptriangle(surface, u1, v1, u4, v1, u2, v2, ZBuffer, color);
			}
			else {
				//fillbottomtriangle(surface, u0, v0, u4, v1, u1, v1, ZBuffer, color);
				//filltoptriangle(surface, u4, v1, u1, v1, u2, v2, ZBuffer, color);
			}

		}
	}

	static void drawtriangle(SDL_Surface* surface, int x0, int y0, int x1, int y1, int x2, int y2, double w, double a_prime, double b_prime, double c_prime, double d, std::vector<double>&  ZBuffer,Uint32 color = 0)
	{
		drawline(surface, x0, y0, x1, y1,  w,  a_prime, b_prime, c_prime, d ,ZBuffer, color);
		drawline(surface, x1, y1, x2, y2, w, a_prime, b_prime, c_prime, d ,  ZBuffer, color);
		drawline(surface, x0, y0, x2, y2, w, a_prime, b_prime, c_prime, d , ZBuffer, color);
	}

	static void drawline(SDL_Surface* surface, int x0, int y0, int x1, int y1, double w, double a_prime, double b_prime, double c_prime, double d, std::vector<double>&  ZBuffer, Uint32 color = 0xFFFFFF)
	{
		for (double t = 0.; t < 1.0; t += .01) {
			int x = x0 + (x1 - x0)*t;
			int y = y0 + (y1 - y0)*t;
			double zpos_camspace_inv = ((a_prime * x) + (b_prime * y) + c_prime);
			double zpos_ndc = zpos_camspace_inv * w;

			if (ZBuffer[x + 800 * y] > zpos_ndc) {
				ZBuffer[x + 800 * y] = zpos_ndc;
				putpixel(surface, x, y, color);
			}
		}
	}

private:
	static void fillflatbottomtriangle_gouraud(SDL_Surface* surface, double p1_x, double p1_y, double p2_x, double p2_y, double p4_x, double p3_y, double w, double a_prime, double b_prime, double c_prime, double d , std::vector<double>& ZBuffer
		, Uint32 p1_color, Uint32 p2_color,Uint32 p3_color)
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
		}

		double dy = p2_y - p1_y;	
		double dx1 = p2_x - p1_x;	
		double dx2 = p4_x - p1_x;

		double slope_1 = 0; double slope_2 = 0;

		if (dy) {
			slope_1 = dx1 / dy;
		}

		if (dy) {
			slope_2 = dx2 / dy;
		}

		double Ip[4] , Ib[4], Ia[4];

			for (int scanline = p1_y; scanline <= p2_y; scanline++) {
				double px1 = slope_1 * (double)(scanline - p2_y) + (double)p2_x;
				double px2 = slope_2 * (double)(scanline - p2_y) + (double)p4_x;
				const int xStart = (int)px1;
				const int xEnd = (int)px2;

				//goroud calculation : https://www.youtube.com/watch?v=06p86OrTGLc&t=233s&ab_channel=raviramamoorthi
				//vertex side intensity
				//Ia = (p1_vertex * (scanline - p2_y) + v2_vertex * (p1_y - scanline)) / (p1_y - p2_y);
				if ((p1_y - p2_y) != 0) {
					double t1 = (double)(scanline - p2_y) / (double)(p1_y - p2_y);
					double t2 = (double)(p1_y - scanline) / (double)(p1_y - p2_y);
					Ia[1] = ((p1_color & 0x00FF0000) >> 16) * t1 + ((p2_color & 0x00FF0000) >> 16) * t2;
					Ia[2] = ((p1_color & 0x0000FF00) >> 8) * t1 + ((p2_color & 0x0000FF00) >> 8) * t2;
					Ia[3] = (p1_color & 0x000000FF) * t1 + (p2_color & 0x000000FF) * t2;
				}

				if ((p1_y - p3_y) != 0) {
					double t1 = (double)(scanline - p3_y) / (double)(p1_y - p3_y);
					double t2 = (double)(p1_y - scanline) / (double)(p1_y - p3_y);
					//Ib = (p1_vertex * (scanline - p3_y) + v3_vertex * (p1_y - scanline)) / (p1_y - p3_y);
				
					Ib[1] = ((p1_color & 0x00FF0000) >> 16) *  t1 + ((p3_color & 0x00FF0000) >> 16) * t2;
					Ib[2] = ((p1_color & 0x0000FF00) >> 8) *  t1 + ((p3_color & 0x0000FF00) >> 8) *  t2;
					Ib[3] = (p1_color & 0x000000FF) *  t1 + (p3_color & 0x000000FF) *  t2;
				}

				if (left_side_major)
				{
					std::swap(Ia, Ib);
				}

				for (int x = xStart; x <= xEnd; ++x) {

					double zpos_camspace_inv = ((a_prime * x) + (b_prime * scanline) + c_prime);
					double zpos_ndc = zpos_camspace_inv * w;
					if (ZBuffer[x + 800 * scanline] > zpos_ndc) {
						ZBuffer[x + 800 * scanline] = zpos_ndc;
					if ((xStart - xEnd) == 0)	//if the point is at the vertex
					{
						Ip[0] = (p1_color & 0xFF000000) >> 24;	
						Ip[1] = (p1_color & 0x00FF0000) >> 16;
						Ip[2] = (p1_color & 0x0000FF00) >> 8;
						Ip[3] = (p1_color & 0x000000FF);
					}

					else
					{
						double t1 = (double)(xEnd - x) / (double)(xEnd - xStart);
						double t2 = (double)(x - xStart) / (double)(xEnd - xStart);
						//Ip = ( Ia * (xEnd - x) + Ib * (x - xStart) ) / (xEnd - xStart);
						Ip[0] = (Ia[0] * t1) + (Ib[0] * t2);
						Ip[1] = (Ia[1] * t1) + (Ib[1] * t2);
						Ip[2] = (Ia[2] * t1) + (Ib[2] * t2);
						Ip[3] = (Ia[3] * t1) + (Ib[3] * t2);
					}

					//Uint32 goroudcol = (Ip[0] << 24) + (Ip[1] << 16) + (Ip[2] << 8) + Ip[3];
					putpixel(surface, x, scanline, ((Uint8)Ip[0] << 24) + ((Uint8)Ip[1] << 16) + ((Uint8)Ip[2] << 8) + (Uint8)Ip[3]);
					}
				}
			}
	}

	static void fillflattoptriangle_gouraud(SDL_Surface* surface, double p2_x, double p2_y, double p4_x, double p1_y, double p3_x, double p3_y, double w, double a_prime, double b_prime, double c_prime, double d, std::vector<double>&  ZBuffer,
		Uint32 p1_color, Uint32 p2_color,Uint32 p3_color)
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
		}

		double dy = p3_y - p2_y;
		double dx1 = p3_x - p2_x;
		double dx2 = p3_x - p4_x;

		double slope_1 = 0; double slope_2 = 0;

		if (dy) {
			slope_1 = dx1 / dy;
		}

		if (dy) {
			slope_2 = dx2 / dy;
		}

		double  Ip[4], Ib[4], Ia[4];

		for (int scanline = p3_y; scanline >= p2_y; scanline--) {
			double px1 = slope_1 * (double)(scanline - p2_y) + (double)p2_x;
			double px2 = slope_2 * (double)(scanline - p3_y) + (double)p3_x;

			const int xStart = (int)px1;
			const int xEnd = (int)px2;
			
			if (p3_y - p2_y) {
				double t1 = (double)(scanline - p2_y) / (double)(p3_y - p2_y);
				double t2 = (double)(p3_y - scanline) / (double)(p3_y - p2_y);
				Ia[1] = ((p3_color & 0x00FF0000) >> 16) * t1 + ((p2_color & 0x00FF0000) >> 16) * t2;
				Ia[2] = ((p3_color & 0x0000FF00) >> 8) * t1 + ((p2_color & 0x0000FF00) >> 8) * t2;
				Ia[3] = (p3_color & 0x000000FF) * t1 + (p2_color & 0x000000FF) * t2;
			}

			if (p3_y - p1_y) {
				double t1 = (double)(scanline - p1_y) / (double)(p3_y - p1_y);
				double t2 = (double)(p3_y - scanline) / (double)(p3_y - p1_y);
				
				//Ib = (p1_vertex * (scanline - p3_y) + v3_vertex * (p1_y - scanline)) / (p1_y - p3_y);
				Ib[1] = ((p3_color & 0x00FF0000) >> 16) *  t1 + ((p1_color & 0x00FF0000) >> 16) * t2;
				Ib[2] = ((p3_color & 0x0000FF00) >> 8) *  t1 + ((p1_color & 0x0000FF00) >> 8) *  t2;
				Ib[3] = (p3_color & 0x000000FF) *  t1 + (p1_color & 0x000000FF) *  t2;
			}

			if (left_side_major)
			{
				std::swap(Ia, Ib);
			}

			for (int x = xStart; x <= xEnd; ++x) {

				double zpos_camspace_inv = ((a_prime * x) + (b_prime * scanline) + c_prime);
				double zpos_ndc = zpos_camspace_inv * w;

			if (ZBuffer[x + 800 * scanline] > zpos_ndc) {
				ZBuffer[x + 800 * scanline] = zpos_ndc;

				if ((xStart - xEnd) == 0)	//if the point is at the vertex
				{
					Ip[0] = (p1_color & 0xFF000000) >> 24;
					Ip[1] = (p1_color & 0x00FF0000) >> 16;
					Ip[2] = (p1_color & 0x0000FF00) >> 8;
					Ip[3] = (p1_color & 0x000000FF);
				}

				else
				{
					double t1 = (double)(xEnd - x) / (double)(xEnd - xStart);
					double t2 = (double)(x - xStart) / (double)(xEnd - xStart);
					//Ip = ( Ia * (xEnd - x) + Ib * (x - xStart) ) / (xEnd - xStart);
					Ip[0] = (Ia[0] * t1) + (Ib[0] * t2);
					Ip[1] = (Ia[1] * t1) + (Ib[1] * t2);
					Ip[2] = (Ia[2] * t1) + (Ib[2] * t2);
					Ip[3] = (Ia[3] * t1) + (Ib[3] * t2);
				}
				putpixel(surface, x, scanline, ((Uint8)Ip[0] << 24) + ((Uint8)Ip[1] << 16) + ((Uint8)Ip[2] << 8) + (Uint8)Ip[3]);
			}
			}
		}
	}

	static void fillflattoptriangle_flat(SDL_Surface* surface, double p2_x, double p2_y, double p4_x, double p1_y, double p3_x, double p3_y, double w, double a_prime, double b_prime, double c_prime, double d, std::vector<double>&  ZBuffer,
		Uint32 color)
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
		}

		double dy = p3_y - p2_y;
		double dx1 = p3_x - p2_x;
		double dx2 = p3_x - p4_x;

		double slope_1 = 0; double slope_2 = 0;

		if (dy) {
			slope_1 = dx1 / dy;
		}

		if (dy) {
			slope_2 = dx2 / dy;
		}

		for (int scanline = p3_y; scanline >= p2_y; scanline--) {
			double px1 = slope_1 * (double)(scanline - p2_y) + (double)p2_x;
			double px2 = slope_2 * (double)(scanline - p3_y) + (double)p3_x;

			const int xStart = (int)px1;
			const int xEnd = (int)px2;

			for (int x = xStart; x <= xEnd; ++x) {

				double zpos_camspace_inv = ((a_prime * x) + (b_prime * scanline) + c_prime);
				double zpos_ndc = zpos_camspace_inv * w;

				if (ZBuffer[x + 800 * scanline] > zpos_ndc) {
					ZBuffer[x + 800 * scanline] = zpos_ndc;
					putpixel(surface, x, scanline, color);
				}

			}
		}
	}

	static void fillflatbottomtriangle_flat(SDL_Surface* surface, double p1_x, double p1_y, double p2_x, double p2_y, double p4_x, double p3_y, double w, double a_prime, double b_prime, double c_prime, double d, std::vector<double>& ZBuffer
		,Uint32 color)
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
		}

		double dy = p2_y - p1_y;
		double dx1 = p2_x - p1_x;
		double dx2 = p4_x - p1_x;

		double slope_1 = 0; double slope_2 = 0;

		if (dy) {
			slope_1 = dx1 / dy;
		}

		if (dy) {
			slope_2 = dx2 / dy;
		}

		for (int scanline = p1_y; scanline <= p2_y; scanline++) {
			double px1 = slope_1 * (double)(scanline - p2_y) + (double)p2_x;
			double px2 = slope_2 * (double)(scanline - p2_y) + (double)p4_x;
			const int xStart = (int)px1;
			const int xEnd = (int)px2;

			for (int x = xStart; x <= xEnd; ++x) {

				double zpos_camspace_inv = ((a_prime * x) + (b_prime * scanline) + c_prime);
				double zpos_ndc = zpos_camspace_inv * w;
				if (ZBuffer[x + 800 * scanline] > zpos_ndc) {
					ZBuffer[x + 800 * scanline] = zpos_ndc;
					putpixel(surface, x, scanline, color);
				}
			}
		}
	}

	static void fillflatbottomtriangle_p(SDL_Surface* surface, double p1_x, double p1_y, double p2_x, double p2_y, double p4_x, double p3_y, double w, double a_prime, double b_prime, double c_prime, double d, std::vector<double>&  ZBuffer,
		std::vector<Vector3f>& vertexnormbuffer, Vector3f& p1_vertex, Vector3f& p2_vertex, Vector3f& p3_vertex,
		Uint32 color = 0xFFFFFF)
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
		}

		double dy = p2_y - p1_y;
		double dx1 = p2_x - p1_x;
		double dx2 = p4_x - p1_x;

		double slope_1 = 0; double slope_2 = 0;

		if (dy) {
			slope_1 = dx1 / dy;
		}

		if (dy) {
			slope_2 = dx2 / dy;
		}

	
		Vector3d Va, Vb, Vp;
		double Vp_x, Vp_y, Vp_z;
		double Va_x, Va_y, Va_z;
		double Vb_x, Vb_y, Vb_z;

		Uint8 light_src_rgba[4], final_light_rgba[4];
		Uint32 final_light;
		light_src_rgba[0] = (color & 0xFF000000) >> 24;
		light_src_rgba[1] = (color & 0x00FF0000) >> 16;
		light_src_rgba[2] = (color & 0x0000FF00) >> 8;
		light_src_rgba[3] = (color & 0x000000FF);

		for (int scanline = p1_y; scanline <= p2_y; scanline++) {
			double px1 = slope_1 * (double)(scanline - p2_y) + p2_x;
			double px2 = slope_2 * (double)(scanline - p2_y) + p4_x;

			const int xStart = (int)px1;
			const int xEnd = (int)px2;

			//goroud calculation : https://www.youtube.com/watch?v=06p86OrTGLc&t=233s&ab_channel=raviramamoorthi
			//vertex side intensity
			//Ia = (p1_vertex * (scanline - p2_y) + v2_vertex * (p1_y - scanline)) / (p1_y - p2_y);
			if ((p1_y - p2_y) != 0) {
				double t1 = (double)(scanline - p2_y) / (double)(p1_y - p2_y);
				double t2 = (double)(p1_y - scanline) / (double)(p1_y - p2_y);
				//Ia = (p1_vertex * (scanline - p2_y) + v2_vertex * (p1_y - scanline)) / (p1_y - p2_y);
				//split the vertices for double precision
				double test = (double)p1_vertex.x;
				Va_x = (double)p1_vertex.x * t1 + (double)p2_vertex.x * t2;
				Va_y = (double)p1_vertex.y * t1 + (double)p2_vertex.y * t2;
				Va_z = (double)p1_vertex.z * t1 + (double)p2_vertex.z * t2;
				Va(Va_x, Va_y, Va_z);
			}
			
			if ((p1_y - p3_y) != 0) {
				float t1 = (double)(scanline - p3_y) / (double)(p1_y - p3_y);
				float t2 = (double)(p1_y - scanline) / (double)(p1_y - p3_y);
				//Ib = (p1_vertex * (scanline - p3_y) + v3_vertex * (p1_y - scanline)) / (p1_y - p3_y);
				Vb_x = (double)p1_vertex.x * t1 + (double)p3_vertex.x * t2;
				Vb_y = (double)p1_vertex.y * t1 + (double)p3_vertex.y * t2;
				Vb_z = (double)p1_vertex.z * t1 + (double)p3_vertex.z * t2;
				Vb(Vb_x, Vb_y, Vb_z);
			}

			if (left_side_major)
			{
				std::swap(Va, Vb);
			}

			for (int x = xStart; x <= xEnd; ++x) {

				double zpos_camspace_inv = ((a_prime * x) + (b_prime * scanline) + c_prime);
				double zpos_ndc = zpos_camspace_inv * w;

				if (ZBuffer[x + 800 * scanline] > zpos_ndc) {
					ZBuffer[x + 800 * scanline] = zpos_ndc;
					if ((xStart - xEnd) == 0)	//if the point is at the vertex
					{
						Vp((double)p1_vertex.x, (double)p1_vertex.y, (double)p1_vertex.z);
					}

					else
					{
						double t1 = (double)(xEnd - x) / (double)(xEnd - xStart);
						double t2 = (double)(x - xStart) / (double)(xEnd - xStart);
						//Ip = ( Ia * (xEnd - x) + Ib * (x - xStart) ) / (xEnd - xStart);
						Vp = Va * t1 + Vb * t2;
					}

					double t = Vp.getNormalized().getDotProduct(Vector3d(0.0, 0.0, -1.0).getNormalized());
					if (t < 0.0)
					{
						t = 0.0;
					}
					for (int j = 1; j < 4; ++j)
					{
						final_light_rgba[j] = light_src_rgba[j] * t;
					}

					final_light = (final_light_rgba[0] << 24) + (final_light_rgba[1] << 16) + (final_light_rgba[2] << 8) + final_light_rgba[3];

					//Uint32 goroudcol = (Ip[0] << 24) + (Ip[1] << 16) + (Ip[2] << 8) + Ip[3];
					putpixel(surface, x, scanline, final_light);
				}
			}
		}
	}

	static void fillflattoptriangle_p(SDL_Surface* surface, double p2_x, double p2_y, double p4_x, double p1_y, double p3_x, double p3_y, double w, double a_prime, double b_prime, double c_prime, double d, std::vector<double>&  ZBuffer,
		std::vector<Vector3f>& vertexnormbuffer, Vector3f& p1_vertex, Vector3f& p2_vertex, Vector3f& p3_vertex,
		Uint32 color = 0xFFFFFF)
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
		}

		double dy = p3_y - p2_y;
		double dx1 = p3_x - p2_x;
		double dx2 = p3_x - p4_x;

		double slope_1 = 0; double slope_2 = 0;

		if (dy) {
			slope_1 = dx1 / dy;
		}

		if (dy) {
			slope_2 = dx2 / dy;
		}

		Vector3d Va, Vb, Vp;
		double Vp_x, Vp_y, Vp_z;
		double Va_x, Va_y, Va_z;
		double Vb_x, Vb_y, Vb_z;

		Uint8 light_src_rgba[4], final_light_rgba[4];
		Uint32 final_light;
		light_src_rgba[0] = (color & 0xFF000000) >> 24;
		light_src_rgba[1] = (color & 0x00FF0000) >> 16;
		light_src_rgba[2] = (color & 0x0000FF00) >> 8;
		light_src_rgba[3] = (color & 0x000000FF);

		for (int scanline = p3_y; scanline >= p2_y; scanline--) {
			double px1 = slope_1 * (double)(scanline - p2_y) + p2_x;
			double px2 = slope_2 * (double)(scanline - p3_y) + p3_x;

			const int xStart = (int)px1;
			const int xEnd = (int)px2;

			if (p3_y - p2_y) {
				double t1 = (double)(scanline - p2_y) / (double)(p3_y - p2_y);
				double t2 = (double)(p3_y - scanline) / (double)(p3_y - p2_y);
				Va_x = (double)p3_vertex.x * t1 + (double)p2_vertex.x * t2;
				Va_y = (double)p3_vertex.y * t1 + (double)p2_vertex.y * t2;
				Va_z = (double)p3_vertex.z * t1 + (double)p2_vertex.z * t2;
			
				Va(Va_x, Va_y, Va_z);
			}

			if (p3_y - p1_y) {
				double t1 = (double)(scanline - p1_y) / (double)(p3_y - p1_y);
				double t2 = (double)(p3_y - scanline) / (double)(p3_y - p1_y);
				Vb_x = (double)p1_vertex.x * t1 + (double)p3_vertex.x * t2;
				Vb_y = (double)p1_vertex.y * t1 + (double)p3_vertex.y * t2;
				Vb_z = (double)p1_vertex.z * t1 + (double)p3_vertex.z * t2;
				Vb(Vb_x, Vb_y, Vb_z);
			}

			if (left_side_major)
			{
				std::swap(Va, Vb);
			}

			for (int x = xStart; x <= xEnd; ++x) {

				double zpos_camspace_inv = ((a_prime * x) + (b_prime * scanline) + c_prime);
				double zpos_ndc = zpos_camspace_inv * w;
				if (ZBuffer[x + 800 * scanline] > zpos_ndc) {
					ZBuffer[x + 800 * scanline] = zpos_ndc;

					if ((xStart - xEnd) == 0)	//if the point is at the vertex
					{
						Vp((double)p1_vertex.x , (double)p1_vertex.y , (double)p1_vertex.z);
					}

					else
					{
						double t1 = (double)(xEnd - x) / (double)(xEnd - xStart);
						double t2 = (double)(x - xStart) / (double)(xEnd - xStart);
						Vp =  Va * t1 + Vb * t2;
					}
					//clacualte colour intensity with vp
					//directional light
					double t = Vp.getNormalized().getDotProduct(Vector3d(0.0, 0.0, -1.0).getNormalized());
					if (t < 0.0)
					{
						t = 0.0;
					}
					for (int j = 1; j < 4; ++j)
					{
						final_light_rgba[j] = light_src_rgba[j] * t;
					}

					final_light = (final_light_rgba[0] << 24) + (final_light_rgba[1] << 16) + (final_light_rgba[2] << 8) + final_light_rgba[3];
					putpixel(surface, x, scanline, final_light);
				}

			}
		}
	}

};