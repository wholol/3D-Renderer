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
	static void filltriangle_phong_flat(SDL_Surface* surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p3_x, double p3_y, double p3_z,
		std::vector<double>& ZBuffer, Vector3f v1_vertex, Vector3f v2_vertex, Vector3f v3_vertex, Vector3f lightdir, Uint32 color = 0xFFFFFF)
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

		if (p1_y == p2_y)
		{
			fillflattoptriangle_phong_flat(surface, p1_x, p1_y, p1_z, p2_x, p2_y, p2_z, p3_x, p3_y, p3_z, ZBuffer, v1_vertex, v2_vertex, v3_vertex, lightdir, color);
		}

		else if (p2_y == p3_y)
		{
			fillflatbottomtriangle_phong_flat(surface, p1_x, p1_y, p1_z, p2_x, p2_y, p2_z, p3_x, p3_y, p3_z, ZBuffer, v1_vertex, v2_vertex, v3_vertex, lightdir, color);
		}

		else {

			int x4 = (double)p1_x + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (double)(p3_x - p1_x);		//determine the oposite end of the triangle bottom/top
			double z4 = p1_z + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (double)(p3_z - p1_z);

			double t = (double)(p3_y - p2_y) / (double)(p3_y - p1_y);
			Vector3f v4_vertex = v3_vertex * (1.0-t) + v1_vertex * t;
			v4_vertex.Normalize();

			//right side major by default perform a swap between x4 and p2_x in the functions if left side.
			//note that p2y is the middle, and it will be used for both sides).
			fillflatbottomtriangle_phong_flat(surface, p1_x, p1_y, p1_z, p2_x, p2_y, p2_z, x4, p3_y, z4, ZBuffer, v1_vertex, v2_vertex, v4_vertex, lightdir, color);
			fillflattoptriangle_phong_flat(surface, p2_x, p2_y, p2_z, x4, p1_y, z4, p3_x, p3_y, p3_z, ZBuffer, v4_vertex, v2_vertex, v3_vertex, lightdir, color);
		}
	}

	static void filltriangle_phong_point(SDL_Surface* surface, int p1_x, int p1_y, int p2_x, int p2_y, int p3_x, int p3_y, Vector3f& camerapos, double w, double a_prime, double b_prime, double c_prime,
		double d, std::vector<double>& ZBuffer, Mat3f Mat, Vector3f v1_vertex, Vector3f v2_vertex, Vector3f v3_vertex, PointLightSetup& pl, Uint32 color = 0xFFFFFF)
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

		if (p1_y == p2_y)
		{
			fillflattoptriangle_phong_point(surface, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, camerapos, w, a_prime, b_prime, c_prime, d, ZBuffer, Mat, v1_vertex, v2_vertex, v3_vertex, pl, color);
		}

		else if (p2_y == p3_y)
		{
			fillflatbottomtriangle_phong_point(surface, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y,camerapos, w,a_prime, b_prime, c_prime, d, ZBuffer, Mat, v1_vertex, v2_vertex, v3_vertex, pl, color);
		}

		else {
			int x4 = (double)p1_x + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (double)(p3_x - p1_x);		//determine the oposite end of the triangle bottom/top
			double t = (double)(p3_y - p2_y) / (double)(p3_y - p1_y);
			Vector3f v4_vertex = v3_vertex * (1.0 - t) + v1_vertex * t;
			
			v4_vertex.Normalize();

			//right side major by default perform a swap between x4 and p2_x in the functions if left side.
			//note that p2y is the middle, and it will be used for both sides).
			fillflatbottomtriangle_phong_point(surface, p1_x, p1_y, p2_x, p2_y, x4, p3_y,camerapos, w, a_prime, b_prime, c_prime, d, ZBuffer, Mat, v1_vertex, v2_vertex, v4_vertex, pl, color);
			fillflattoptriangle_phong_point(surface, p2_x, p2_y, x4, p1_y, p3_x, p3_y,camerapos, w, a_prime, b_prime, c_prime, d, ZBuffer, Mat, v4_vertex, v2_vertex, v3_vertex, pl, color);
		}
	}


	static void filltriangle_gouraud(SDL_Surface* surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p3_x, double p3_y, double p3_z, std::vector<double>& ZBuffer,Uint32 p1_color, Uint32 p2_color, Uint32 p3_color)
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
			fillflattoptriangle_gouraud(surface, p1_x, p1_y,p1_z, p2_x, p2_y,p2_z,p3_x, p3_y ,p3_z,ZBuffer, p1_color, p2_color, p3_color);
		}

		else if (p2_y == p3_y)
		{
			fillflatbottomtriangle_gouraud(surface, p1_x, p1_y, p1_z, p2_x, p2_y, p2_z, p3_x, p3_y, p3_z,ZBuffer, p1_color, p2_color, p3_color);
		}

		else {
			//lerp end point opposite of vertex
			double x4 = p1_x + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (p3_x - p1_x);		//determine the oposite end of the triangle bottom/top
			double t = (double)(p3_y - p2_y) / (double)(p3_y - p1_y);

			double z4 = p1_z + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (double)(p3_z - p1_z);
			//lerp color
			Uint8 p4_color_rgba[4];
			Uint32 p4_color;
			p4_color_rgba[0] = (((p3_color & 0x00FF0000) >> 24) * (1-t)) + (((p1_color & 0x00FF0000) >> 24) * t);
			p4_color_rgba[1] = (((p3_color & 0x00FF0000) >> 16) * (1 - t)) + (((p1_color & 0x00FF0000) >> 16) * t);
			p4_color_rgba[2] = (((p3_color & 0x0000FF00) >> 8) * (1 - t)) + (((p1_color & 0x0000FF00) >> 8) * t);
			p4_color_rgba[3] = (((p3_color & 0x000000FF)) * (1 - t)) + (((p1_color & 0x000000FF)) * t);
			p4_color = (p4_color_rgba[0] << 24) + (p4_color_rgba[1] << 16) + (p4_color_rgba[2] << 8) + (p4_color_rgba[3]);
			//right side major by default perform a swap between x4 and p2_x in the functions if left side.
			//note that p2y is the middle, and it will be used for both sides).
			
			fillflatbottomtriangle_gouraud(surface, p1_x, p1_y,p1_z, p2_x, p2_y,p2_z, x4, p3_y,z4, ZBuffer, p1_color, p2_color, p4_color);
			fillflattoptriangle_gouraud(surface, p2_x, p2_y,p2_z, x4, p1_y, z4, p3_x, p3_y, p3_z, ZBuffer, p4_color, p2_color, p3_color);
		}
	}

	static void filltriangle_flat(SDL_Surface* surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y,double p2_z, double p3_x, double p3_y, double p3_z, std::vector<double>& ZBuffer, Uint32 color)
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

		if(p1_y == p2_y)
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

	static void drawtriangle(SDL_Surface* surface, int x0, int y0, double z0, int x1, int y1, double z1, int x2, int y2, double z2,std::vector<double>& ZBuffer,Uint32 color = 0)
	{
		drawline(surface, x0, y0, z0, x1, y1, z1,ZBuffer, color);
		drawline(surface, x1, y1,z1, x2, y2,z2, ZBuffer, color);
		drawline(surface, x0, y0,z0, x2, y2,z2, ZBuffer, color);
	}

	static void drawline(SDL_Surface* surface, int x0, int y0,double z0, int x1, int y1,double z1, std::vector<double>&  ZBuffer, Uint32 color = 0xFFFFFF)
	{
		for (double t = 0.; t < 1.0; t += .01) {
			int x = x0 + (x1 - x0)*t;
			int y = y0 + (y1 - y0)*t;
			if (x <= 0) x = 0;
			if (x >= 799) x = 799;
			if (y <= 0) y = 0;
			if (y >= 599) y = 599;
			
			double z = z0 + (z1 - z0)*t;

			if (ZBuffer[x + 800 * y] > z) {
				ZBuffer[x + 800 * y] = z;
				putpixel(surface, x, y, color);
			}
		}
	}

private:
	static void fillflatbottomtriangle_gouraud(SDL_Surface* surface, double p1_x, double p1_y,double p1_z, double p2_x, double  p2_y, double p2_z,double p4_x, double p3_y, double p4_z, std::vector<double>& ZBuffer
		, Uint32 p1_color, Uint32 p2_color, Uint32 p3_color)
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
		if (p1_y <= 1) p1_y = 1;
		if (p2_y >= 599) p2_y = 599;
		
		int yStart = (int)p1_y;
		int yEnd = (int)p2_y;

		for (int scanline = yStart; scanline <= yEnd; scanline++) {

			double px1 = slope_1 * ((double)scanline - p2_y) + p2_x;
			double px2 = slope_2 * ((double)scanline - p2_y) + p4_x;
			
			double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
			double zEnd = slope_2_z * ((double)scanline - p2_y) + p4_z;

			int xStart = (int)px1;
			int xEnd = (int)px2;

			if (xStart <= 1) xStart = 1;
			if (xEnd >= 799) xEnd = 799;

			//goroud calculation : https://www.youtube.com/watch?v=06p86OrTGLc&t=233s&ab_channel=raviramamoorthi
			//vertex side intensity
			//Ia = (p1_vertex * (scanline - p2_y) + v2_vertex * (p1_y - scanline)) / (p1_y - p2_y);
			double t = ((double)scanline - p2_y) / (p1_y - p2_y);
			
			Ia[1] = ((p1_color & 0x00FF0000) >> 16) * t + ((p2_color & 0x00FF0000) >> 16) * (1.0 - t);
			Ia[2] = ((p1_color & 0x0000FF00) >> 8) * t + ((p2_color & 0x0000FF00) >> 8) * (1.0 - t);
			Ia[3] = (p1_color & 0x000000FF) * t + (p2_color & 0x000000FF) *  (1.0 - t);

			//Ib = (p1_vertex * (scanline - p3_y) + v3_vertex * (p1_y - scanline)) / (p1_y - p3_y);
			Ib[1] = ((p1_color & 0x00FF0000) >> 16) *  t + ((p3_color & 0x00FF0000) >> 16) * (1.0 - t);
			Ib[2] = ((p1_color & 0x0000FF00) >> 8) *  t + ((p3_color & 0x0000FF00) >> 8) *  (1.0 - t);
			Ib[3] = (p1_color & 0x000000FF) *  t + (p3_color & 0x000000FF) *  (1.0 - t);

			if (left_side_major)
			{
				std::swap(Ia, Ib);
			}

			for (int x = xStart; x <= xEnd; ++x) {
			
				double t = (double)(xEnd - x) / (double)(xEnd - xStart);
				double z_frag = zStart * t + zEnd * (1.0 - t);
			
				if (ZBuffer[x + 800 * scanline] > z_frag) {
					ZBuffer[x + 800 * scanline] = z_frag;
					if ((xStart - xEnd) == 0)	//if the point is at the vertex
					{
						Ip[0] = (p1_color & 0xFF000000) >> 24;
						Ip[1] = (p1_color & 0x00FF0000) >> 16;
						Ip[2] = (p1_color & 0x0000FF00) >> 8;
						Ip[3] = (p1_color & 0x000000FF);
					}

					else
					{
						for (int i = 0; i < 4; ++i)
						{
							Ip[i] = (Ia[i] * t) + (Ib[i] * (1.0 - t));
						}
					}
					putpixel(surface, x, scanline, ((Uint8)Ip[0] << 24) + ((Uint8)Ip[1] << 16) + ((Uint8)Ip[2] << 8) + (Uint8)Ip[3]);
				}
			}
		}
	}

	static void fillflattoptriangle_gouraud(SDL_Surface* surface, double p2_x, double p2_y,double p2_z, double p4_x, double p1_y,double p4_z, double p3_x, double p3_y,double p3_z, std::vector<double>&  ZBuffer,
		Uint32 p1_color, Uint32 p2_color, Uint32 p3_color)
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
		if (p2_y <= 1) p2_y = 1;
		if (p3_y >= 599) p3_y = 599;

		int yStart = (int)p3_y;
		int yEnd = (int)p2_y;

		for (int scanline = yStart; scanline > yEnd; scanline--) {
			double px1 = slope_1 * ((double)scanline - p2_y) + p2_x;
			double px2 = slope_2 * ((double)scanline - p3_y) + p3_x;

			double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
			double zEnd = slope_2_z * ((double)scanline - p3_y) + p3_z;

			int xStart = (int)px1;
			int xEnd = (int)px2;
			if (xStart <= 1) xStart = 1;
			if (xEnd >= 799) xEnd = 799;
			
			double t = ((double)scanline - p2_y) / (p3_y - p2_y);
			
			Ia[1] = ((p3_color & 0x00FF0000) >> 16) * t + ((p2_color & 0x00FF0000) >> 16) * (1.0 - t);
			Ia[2] = ((p3_color & 0x0000FF00) >> 8) * t + ((p2_color & 0x0000FF00) >> 8) * (1.0 - t);
			Ia[3] = (p3_color & 0x000000FF) * t + (p2_color & 0x000000FF) * (1.0 - t);

			//Ib = (p1_vertex * (scanline - p3_y) + v3_vertex * (p1_y - scanline)) / (p1_y - p3_y);
			Ib[1] = ((p3_color & 0x00FF0000) >> 16) *  t + ((p1_color & 0x00FF0000) >> 16) * (1.0 - t);
			Ib[2] = ((p3_color & 0x0000FF00) >> 8) *  t + ((p1_color & 0x0000FF00) >> 8) *  (1.0 - t);
			Ib[3] = (p3_color & 0x000000FF) *  t + (p1_color & 0x000000FF) * (1.0 - t);

			if (left_side_major)
			{
				std::swap(Ia, Ib);
			}

			for (int x = xStart; x <= xEnd; ++x) {

				double t = (double)(xEnd - x) / (double)(xEnd - xStart);
				double z_frag = zStart * t + zEnd * (1.0 - t);

				if (ZBuffer[x + 800 * scanline] > z_frag) {
					ZBuffer[x + 800 * scanline] = z_frag;

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

	static void fillflatbottomtriangle_flat(SDL_Surface* surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p4_x, double p3_y, double p4_z, std::vector<double>& ZBuffer
		, Uint32 color)
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


		if (p1_y <= 0) { p1_y = 0; }
		if (p2_y >= 599) { p2_y = 599; }

		for (int scanline = p1_y; scanline <= p2_y; scanline++) {
			double px1 = slope_1 * ((double)scanline - p2_y) + p2_x;
			double px2 = slope_2 * ((double)scanline - p2_y) + p4_x;

			double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
			double zEnd = slope_2_z * ((double)scanline - p2_y) + p4_z;

			int xStart = (int)px1;
			int xEnd = (int)px2;

			if (xStart <= 0) { xStart = 0; }
			if (xEnd >= 799) { xEnd = 799; }

			for (int x = xStart; x <= xEnd; ++x) {

				double t = (double)(xEnd - x) / (double)(xEnd - xStart);
				double z_frag = zStart * t + zEnd * (1.0 - t);

				if (ZBuffer[x + 800 * scanline] > z_frag) {
					ZBuffer[x + 800 * scanline] = z_frag;
					putpixel(surface, x, scanline, color);
				}
			}
		}
	}

	static void fillflattoptriangle_flat(SDL_Surface* surface, double p2_x, double p2_y,double p2_z, double p4_x, double p1_y, double p4_z, double p3_x, double p3_y, double p3_z, std::vector<double>& ZBuffer,
		Uint32 color)
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

		if (p2_y <= 0) { p2_y = 0; }
		if (p3_y >= 599) { p3_y = 599; }

		for (int scanline = p3_y; scanline >= p2_y; scanline--) {
			double px1 = slope_1 * ((double)scanline - p2_y) + p2_x;
			double px2 = slope_2 * ((double)scanline - p3_y) + p3_x;
			
			double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
			double zEnd = slope_2_z * ((double)scanline - p3_y) + p3_z;

			int xStart = (int)px1;
			int xEnd = (int)px2;

			if (xStart <= 0) { xStart = 0; }
			if (xEnd >= 799) { xEnd = 799; }

			for (int x = xStart; x <= xEnd; ++x) {
				
				double t = (double)(xEnd - x) / (double)(xEnd - xStart);
				double z_frag = zStart * t + zEnd * (1.0 - t);
				
				if (ZBuffer[x + 800 * scanline] > z_frag) {
					ZBuffer[x + 800 * scanline] = z_frag;
					putpixel(surface, x, scanline, color);
				}
			}
		}
	}

	static void fillflatbottomtriangle_phong_flat(SDL_Surface* surface, double p1_x, double p1_y, double p1_z, double p2_x, double p2_y, double p2_z, double p4_x, double p3_y, double p4_z, std::vector<double>& ZBuffer, Vector3f& p1_vertex, Vector3f& p2_vertex, Vector3f& p3_vertex,
		Vector3f& lightdir,Uint32 color)
	{
		//by default:
		//p1_x , p1_y = top of flat botom triangle
		//p2_x , p2_y = end of flat bottom triangle (vertex side)
		//p4_x , p2_y = end of flat bottom triangle (non vertex side)
		//p3_y = for end of overall triangle - used for interpolation 	
		bool left_side_major = false;

		if (p2_x > p4_x)	//if the triangle is left side major, i.e. non vertex side xpos < vertex side xpos
		{
			left_side_major = true;
			std::swap(p4_x, p2_x);
			std::swap(p2_vertex, p3_vertex);
		}

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

		
		Vector3f Va, Vb, Vp;
		float Vp_x, Vp_y, Vp_z;
		float Va_x, Va_y, Va_z;
		float Vb_x, Vb_y, Vb_z;

		Uint8 light_src_rgba[4], final_light_rgba[4];
		Uint32 final_light;
		light_src_rgba[0] = (color & 0xFF000000) >> 24;
		light_src_rgba[1] = (color & 0x00FF0000) >> 16;
		light_src_rgba[2] = (color & 0x0000FF00) >> 8;
		light_src_rgba[3] = (color & 0x000000FF);

		int yStart = (int)p1_y;
		int yEnd = (int)p2_y;

		if (yStart <= 1) { yStart = 1; }
		if (yEnd >= 599) { yEnd = 599; }
		
		for (int scanline = yStart; scanline <= yEnd; scanline++) {
			double px1 = slope_1 * ((double)scanline - p2_y) + p2_x;
			double px2 = slope_2 * ((double)scanline - p2_y) + p4_x;

			double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
			double zEnd = slope_2_z * ((double)scanline - p2_y) + p4_z;

			int xStart = (int)px1;
			int xEnd = (int)px2;

			if (xStart <= 1) { xStart = 1; }
			if (xEnd >= 799) { xEnd = 799; }

			double t = ((double)scanline - p2_y) / (p1_y - p2_y);
			
			//LHS
			Va_x = p1_vertex.x * t + p2_vertex.x * (1.0 - t);
			Va_y = p1_vertex.y * t + p2_vertex.y * (1.0 - t);
			Va_z = p1_vertex.z * t + p2_vertex.z * (1.0 - t);
			Va(Va_x, Va_y, Va_z);
			Va.Normalize();
			
			//RHS
			Vb_x =  p1_vertex.x * t + p3_vertex.x * (1.0 - t);
			Vb_y =  p1_vertex.y * t + p3_vertex.y * (1.0 - t);
			Vb_z =  p1_vertex.z * t + p3_vertex.z * (1.0 - t);
			Vb(Vb_x, Vb_y, Vb_z);
			Vb.Normalize();

			if (left_side_major)
			{
				std::swap(Va, Vb);
			}

			for (int x = xStart; x <= xEnd; ++x) {
				
				double t_x = (double)(xEnd - x) / (double)(xEnd - xStart);

				double z_frag = zStart * t_x + zEnd * (1.0 - t_x);
				
				if (ZBuffer[x + 800 * scanline] > z_frag) {
					ZBuffer[x + 800 * scanline] = z_frag;
					
					if ((xStart - xEnd) == 0)	//if the point is at the begining
					{
						Vp(p1_vertex.x , p1_vertex.y , p1_vertex.z );
					}

					else
					{
						Vp = Va * (1.0 - t_x) + Vb * t_x;
					}

					double t = Vp.Normalize().getDotProduct(lightdir.getNormalized());

					if (t < 0.0)
					{
						t = 0.0;
					}

					for (int j = 1; j < 4; ++j)
					{
						final_light_rgba[j] = Uint8(light_src_rgba[j] * t);
					}
					
					final_light = (final_light_rgba[0] << 24) + (final_light_rgba[1] << 16) + (final_light_rgba[2] << 8) + final_light_rgba[3];
					putpixel(surface, x, scanline, final_light);
				}
				
#ifdef _DEBUG

				if (scanline == yEnd  && x == xEnd)
				{
					std::cout << "flat bot" << std::endl;
					std::cout << "yEnd: " << yEnd << "xEnd: " << xEnd << std::endl;
					std::cout << Vp.x << " " << Vp.y << " " << Vp.z << std::endl;
					std::cout << p3_vertex.x << " " << p3_vertex.y << " " << p3_vertex.z << std::endl;
					std::cout << p2_vertex.x << " " << p2_vertex.y << " " << p2_vertex.z << std::endl;
					//std::cout << Va.x << " " << Va.y << " " << Va.z << std::endl;
					//std::cout << Vb.x << " " << Vb.y << " " << Vb.z << std::endl;
				}
#else
#endif
			}
		}
	
	}

	static void fillflattoptriangle_phong_flat(SDL_Surface* surface, double p2_x, double p2_y, double p2_z, double p4_x, double p1_y, double p4_z, double p3_x, double p3_y, double p3_z, std::vector<double>& ZBuffer, Vector3f& p1_vertex, Vector3f& p2_vertex, Vector3f& p3_vertex, Vector3f lightdir,
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
			std::swap(p2_x, p4_x);
			std::swap(p2_z, p4_z);
			left_side_major = true;
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
		if (p3_y >= 599) { p3_y = 599; }

		Vector3f Va, Vb, Vp;
		float Vp_x, Vp_y, Vp_z;
		float Va_x, Va_y, Va_z;
		float Vb_x, Vb_y, Vb_z;

		Uint8 light_src_rgba[4], final_light_rgba[4];
		Uint32 final_light;
		light_src_rgba[0] = (color & 0xFF000000) >> 24;
		light_src_rgba[1] = (color & 0x00FF0000) >> 16;
		light_src_rgba[2] = (color & 0x0000FF00) >> 8;
		light_src_rgba[3] = (color & 0x000000FF);

		int yStart = (int)p3_y;
		int yEnd = (int)p2_y;

		if (yStart >= 599) { yStart = 599; }
		if (yEnd <= 1) { yEnd = 1; }

		for (int scanline = yStart; scanline > yEnd; scanline--) {
			double px1 = slope_1 * ((double)scanline - yEnd) + p2_x;
			double px2 = slope_2 * ((double)scanline - yStart) + p3_x;

			double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
			double zEnd = slope_2_z * ((double)scanline - p3_y) + p3_z;

			int xStart = (int)px1;
			int xEnd = (int)px2;

			if (xStart <= 1) { xStart = 1; }
			if (xEnd >= 799) { xEnd = 799; }
			
			double t = ((double)scanline - p2_y) / (p3_y - p2_y);
			
			//LHS
			Va_x = p3_vertex.x * t + p2_vertex.x * (1.0 - t);
			Va_y = p3_vertex.y * t + p2_vertex.y * (1.0 - t);
			Va_z = p3_vertex.z * t + p2_vertex.z * (1.0 - t);
			Va(Va_x, Va_y, Va_z);
			Va.Normalize();

			//RHS
			Vb_x = p3_vertex.x * t + p1_vertex.x * (1.0 - t);
			Vb_y = p3_vertex.y * t + p1_vertex.y * (1.0 - t);
			Vb_z = p3_vertex.z * t + p1_vertex.z * (1.0 - t);
			Vb(Vb_x, Vb_y, Vb_z);
			Vb.Normalize();

			if (left_side_major)
			{
				std::swap(Va, Vb);
			}
			
			for (int x = xStart; x <= xEnd; ++x) {

				double t_x = (double)(xEnd - x) / (double)(xEnd - xStart);
				double z_frag = zStart * t_x + zEnd * (1.0 - t_x);

				if (ZBuffer[x + 800 * scanline] > z_frag) {
					ZBuffer[x + 800 * scanline] = z_frag;

					if ((xStart - xEnd) == 0)	//if the point is at the vertex
					{
						Vp(p3_vertex.x , p3_vertex.y , p3_vertex.z);
					}

					else
					{
						Vp =  Va * (1.0 - t_x) + Vb * t_x;
					}
					//clacualte colour intensity with vp
					//directional light
					double t = Vp.Normalize().getDotProduct(lightdir.getNormalized());
					if (t < 0.0)
					{
						t = 0.0;
					}

					for (int j = 1; j < 4; ++j)
					{
						final_light_rgba[j] = Uint8(light_src_rgba[j] * t);
					}

					final_light = (final_light_rgba[0] << 24) + (final_light_rgba[1] << 16) + (final_light_rgba[2] << 8) + final_light_rgba[3];
					putpixel(surface, x, scanline, final_light);
				}

#if defined _DEBUG

				if (scanline == yEnd && x == xEnd)
				{
					std::cout << "flat top" << std::endl;
					std::cout << "yEnd: " << yEnd << "xEnd: " << xEnd << std::endl;
					std::cout << Vp.x << " " << Vp.y << " " << Vp.z << std::endl;
					std::cout << p1_vertex.x << " " << p1_vertex.y << " " << p1_vertex.z << std::endl;
					std::cout << p2_vertex.x << " " << p2_vertex.y << " " << p2_vertex.z << std::endl;
				}
#else
#endif
			}
		}
	}

	static void fillflatbottomtriangle_phong_point(SDL_Surface* surface, int p1_x, int p1_y, int p2_x, int p2_y, int p4_x, int p3_y, Vector3f& camerapos, double w, double a_prime, double b_prime, double c_prime, double d, std::vector<double>&  ZBuffer,
		Mat3f& Mat, Vector3f& p1_vertex, Vector3f& p2_vertex, Vector3f& p3_vertex, PointLightSetup& pl,
		Uint32 color = 0xFFFFFF)
	{
		//by default:
		//p1_x , p1_y = top of flat botom triangle
		//p2_x , p2_y = end of flat bottom triangle (vertex side)
		//p4_x , p2_y = end of flat bottom triangle (non vertex side)
		//p3_y = for end of overall triangle - used for interpolation 	
		bool left_side_major = false;

		if (p2_x > p4_x)	//if the triangle is left side major, i.e. non vertex side xpos < vertex side xpos
		{
			left_side_major = true;
			std::swap(p4_x, p2_x);
			std::swap(p2_vertex, p3_vertex);
		}

		int dy = p2_y - p1_y;
		int dx1 = p2_x - p1_x;
		int dx2 = p4_x - p1_x;

		double slope_1 = 0; double slope_2 = 0;

		if (dy) {
			slope_1 = (double)dx1 / (double)dy;
		}

		if (dy) {
			slope_2 = (double)dx2 / (double)dy;
		}

		Vector3f Va, Vb, Vp;
		float Vp_x, Vp_y, Vp_z;
		float Va_x, Va_y, Va_z;
		float Vb_x, Vb_y, Vb_z;

		Uint8 light_src_rgba[4], final_light_rgba[4];
		Uint32 final_light;
		light_src_rgba[0] = (color & 0xFF000000) >> 24;
		light_src_rgba[1] = (color & 0x00FF0000) >> 16;
		light_src_rgba[2] = (color & 0x0000FF00) >> 8;
		light_src_rgba[3] = (color & 0x000000FF);

		int yStart = p1_y;
		int yEnd = p2_y;

		if (yStart <= 1) { yStart = 1; }
		if (yEnd >= 599) { yEnd = 599; }

		for (int scanline = yStart; scanline <= yEnd; scanline++) {
			double px1 = slope_1 * (double)(scanline - yEnd) + p2_x;
			double px2 = slope_2 * (double)(scanline - yEnd) + p4_x;

			int xStart = (int)px1;
			int xEnd = (int)px2;

			if (xStart <= 1) { xStart = 1; }
			if (xEnd >= 799) { xEnd = 799; }

			double t = (double)(scanline - yEnd) / (double)(yStart - yEnd);

			//LHS
			Va_x = p1_vertex.x * t + p2_vertex.x * (1.0 - t);
			Va_y = p1_vertex.y * t + p2_vertex.y * (1.0 - t);
			Va_z = p1_vertex.z * t + p2_vertex.z * (1.0 - t);
			Va(Va_x, Va_y, Va_z);
			Va.Normalize();

			//RHS
			Vb_x = p1_vertex.x * t + p3_vertex.x * (1.0 - t);
			Vb_y = p1_vertex.y * t + p3_vertex.y * (1.0 - t);
			Vb_z = p1_vertex.z * t + p3_vertex.z * (1.0 - t);
			Vb(Vb_x, Vb_y, Vb_z);
			Vb.Normalize();

			for (int x = xStart; x <= xEnd; ++x) {
				double zpos_camspace_inv = ((a_prime * x) + (b_prime * scanline) + c_prime);
				
				double zpos_ndc = (1.0/zpos_camspace_inv) * w;

				if (ZBuffer[x + 800 * scanline] > zpos_ndc) {
					ZBuffer[x + 800 * scanline] = zpos_ndc;

					if ((xStart - xEnd) == 0)	//if the point is at the begining
					{
						Vp(p1_vertex.x, p1_vertex.y, p1_vertex.z);
					}

					else
					{
						double t = (double)(xEnd - x) / (double)(xEnd - xStart);
						Vp = Va * (1.0 - t) + Vb * t;
					}

					//get viewspace coordinates
					Vector3f view(x, scanline, 0);
					view = Mat3f::Scale(1 / (0.5 * 800), 1 / (0.5 * 600), 1) * view;
					view = Mat3f::Translate(-1, -1, 0) * view;
					view = view * (float)w;		//ndc to view space
					view = Mat * view;

					//gotta invert the view mat
					view.z = 1.0 / (zpos_camspace_inv);

					Vector3f to_light = pl.lightpos - view;
					double dist = to_light.getMagnitude();	//get distance from point lgiht to vertex point
					double attenuation = 1.0 / ((pl.a * dist * dist) + (pl.b * dist) + pl.c);	//get attenuation
					to_light.Normalize();	//normalize

					//ambient
					double amb_k = pl.amb_constant;

					//diffuse
					double diff_k;
					diff_k = std::max(0.0f, to_light.getNormalized().getDotProduct(Vp.getNormalized()));

					//specular
					Vector3f ViewVec = camerapos - view;
					Vector3f w = to_light * 2.0 * Vp.getNormalized().getDotProduct(to_light);
					Vector3f r = w - to_light;
					double spec_k = std::max(0.0f, std::powf((ViewVec.getNormalized().getDotProduct(r.getNormalized())), pl.spec_exponent));

					if (spec_k > 1.0f)
					{
						spec_k = 1.0f;
					}

					double f = (amb_k + attenuation * (diff_k + (spec_k * pl.spec_intensity)));

					if (f > 1.0)
					{
						f = 1.0;
					}
						
					for (int j = 1; j < 4; ++j)
					{
						final_light_rgba[j] = Uint8(light_src_rgba[j] * f);
					}

					final_light = (final_light_rgba[0] << 24) + (final_light_rgba[1] << 16) + (final_light_rgba[2] << 8) + final_light_rgba[3];
					putpixel(surface, x, scanline, final_light);
				}

#ifdef _DEBUG

				if (scanline == yEnd && x == xEnd)
				{
					std::cout << "flat bot" << std::endl;
					std::cout << "yEnd: " << yEnd << "xEnd: " << xEnd << std::endl;
					std::cout << Vp.x << " " << Vp.y << " " << Vp.z << std::endl;
					std::cout << p3_vertex.x << " " << p3_vertex.y << " " << p3_vertex.z << std::endl;
					std::cout << p2_vertex.x << " " << p2_vertex.y << " " << p2_vertex.z << std::endl;
					//std::cout << Va.x << " " << Va.y << " " << Va.z << std::endl;
					//std::cout << Vb.x << " " << Vb.y << " " << Vb.z << std::endl;
				}
#else
#endif
			}
		}

	}

	static void fillflattoptriangle_phong_point(SDL_Surface* surface, double p2_x, double p2_y, double p4_x, double p1_y, double p3_x, double p3_y, Vector3f& camerapos, double w, double a_prime, double b_prime, double c_prime, double d, std::vector<double>&  ZBuffer,
		Mat3f& Mat, Vector3f& p1_vertex, Vector3f& p2_vertex, Vector3f& p3_vertex, PointLightSetup& pl,
		Uint32 color = 0xFFFFFF)
	{
		//by default:
		//p2_x , p2_y = top left of flat top triangle (middle vertex coordinate)
		//p4_x , p2_y = end right flat top triangle 
		//p3_x , p3_y = end of flat top triangle 
		//p1_y = for top of overall triangle - used for goroud interpolation of colour intensity 
		if (p2_x > p4_x)
		{
			std::swap(p2_x, p4_x);
			std::swap(p1_vertex, p2_vertex);
		}

		int dy = p3_y - p2_y;
		int dx1 = p3_x - p2_x;
		int dx2 = p3_x - p4_x;

		double slope_1 = 0; double slope_2 = 0;

		if (dy) {
			slope_1 = (double)dx1 / (double)dy;
		}

		if (dy) {
			slope_2 = (double)dx2 / (double)dy;
		}

		Vector3f Va, Vb, Vp;
		float Vp_x, Vp_y, Vp_z;
		float Va_x, Va_y, Va_z;
		float Vb_x, Vb_y, Vb_z;

		Uint8 light_src_rgba[4], final_light_rgba[4];
		Uint32 final_light;
		light_src_rgba[0] = (color & 0xFF000000) >> 24;
		light_src_rgba[1] = (color & 0x00FF0000) >> 16;
		light_src_rgba[2] = (color & 0x0000FF00) >> 8;
		light_src_rgba[3] = (color & 0x000000FF);

		int yStart = (int)p3_y;
		int yEnd = (int)p2_y;

		if (yStart >= 599) { yStart = 599; }
		if (yEnd <= 1) { yEnd = 1; }

		for (int scanline = yStart; scanline >= yEnd; scanline--) {
			double px1 = slope_1 * (double)(scanline - yEnd) + p2_x;
			double px2 = slope_2 * (double)(scanline - yStart) + p3_x;

			int xStart = (int)px1;
			int xEnd = (int)px2;

			if (xStart <= 1) { xStart = 1; }
			if (xEnd >= 799) { xEnd = 799; }

			double t = (double)(scanline - yEnd) / (double)(yStart - yEnd);

			//LHS
			Va_x = p3_vertex.x * t + p2_vertex.x * (1.0 - t);
			Va_y = p3_vertex.y * t + p2_vertex.y * (1.0 - t);
			Va_z = p3_vertex.z * t + p2_vertex.z * (1.0 - t);
			Va(Va_x, Va_y, Va_z);
			Va.Normalize();

			//RHS
			Vb_x = p3_vertex.x * t + p1_vertex.x * (1.0 - t);
			Vb_y = p3_vertex.y * t + p1_vertex.y * (1.0 - t);
			Vb_z = p3_vertex.z * t + p1_vertex.z * (1.0 - t);
			Vb(Vb_x, Vb_y, Vb_z);
			Vb.Normalize();

			for (int x = xStart; x <= xEnd; ++x) {

				double zpos_camspace_inv = ((a_prime * x) + (b_prime * scanline) + c_prime);
				double zpos_ndc = (1.0 / zpos_camspace_inv)  * w;

				if (ZBuffer[x + 800 * scanline] > zpos_ndc) {
					ZBuffer[x + 800 * scanline] = zpos_ndc;

					if ((xStart - xEnd) == 0)	//if the point is at the vertex
					{
						Vp(p3_vertex.x, p3_vertex.y, p3_vertex.z);
					}

					else
					{
						double t = (double)(xEnd - x) / (double)(xEnd - xStart);
						Vp = Va * (1 - t) + Vb * t;
					}

					//get viewspace coordinates
					Vector3f view(x, scanline, 0);
					view = Mat3f::Scale(1 / (0.5 * 800), 1 / (0.5 * 600), 1) * view;
					view = Mat3f::Translate(-1, -1, 0) * view;
					view = view *  (float)w;		//ndc to view space
					view = Mat * view;

					//gotta invert the view mat
					view.z = 1.0 / (zpos_camspace_inv);

					Vector3f to_light = pl.lightpos - view;
					double dist = to_light.getMagnitude();	//get distance from point lgiht to vertex point
					double attenuation = 1.0 / ((pl.a * dist * dist) + (pl.b * dist) + pl.c);	//get attenuation
					to_light.Normalize();	//normalize

					//ambient
					double amb_k = pl.amb_constant;

					//diffuse
					double diff_k;
					diff_k = std::max(0.0f, to_light.getNormalized().getDotProduct(Vp.getNormalized()));

					//specular
					Vector3f ViewVec = camerapos - view;
					Vector3f w = to_light * 2.0 * Vp.getNormalized().getDotProduct(to_light);
					Vector3f r = w - to_light;
					double spec_k = std::max(0.0f, std::powf((ViewVec.getNormalized().getDotProduct(r.getNormalized())), pl.spec_exponent));

					if (spec_k > 1.0)
					{
						spec_k = 1.0;
					}

					double f = (amb_k + attenuation * (diff_k + (spec_k * pl.spec_intensity)));

					if (f > 1.0)
					{
						f = 1.0;
					}

					for (int j = 1; j < 4; ++j)
					{
						final_light_rgba[j] = Uint8(light_src_rgba[j] * f);
					}

					final_light = (final_light_rgba[0] << 24) + (final_light_rgba[1] << 16) + (final_light_rgba[2] << 8) + final_light_rgba[3];
					putpixel(surface, x, scanline, final_light);
				}

#if defined _DEBUG

				if (scanline == yEnd && x == xEnd)
				{
					std::cout << "flat top" << std::endl;
					std::cout << "yEnd: " << yEnd << "xEnd: " << xEnd << std::endl;
					std::cout << Vp.x << " " << Vp.y << " " << Vp.z << std::endl;
					std::cout << p1_vertex.x << " " << p1_vertex.y << " " << p1_vertex.z << std::endl;
					std::cout << p2_vertex.x << " " << p2_vertex.y << " " << p2_vertex.z << std::endl;
				}
#else
#endif
			}
		}
	}
};