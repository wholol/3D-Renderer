#pragma once
#include "SDL.h"
//FRAGMENT PART
#include <algorithm>
class Draw {

public:

	static void putpixel(SDL_Surface *surface, int x, int y, std::vector<float>& ZBuffer,  Uint32 pixel = 0xFFFFFF)
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

	static void filltriangle(SDL_Surface* surface, int p1_x, int p1_y, int p2_x, int p2_y, int p3_x, int p3_y,float w, float a_prime, float b_prime, float c_prime, 
		float d , std::vector<float>& ZBuffer,std::vector<Vector3f>& vertexnormbuffer,Uint32 p1_color, Uint32 p2_color, Uint32 p3_color, Uint32 color = 0xFFFFFF)

		/*fill the triangle such that :
		 p1 is the top vertex, 2 is the middle vertex, p3 is the bottom vertex
		*/
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

		/*if it is only a bottom flat triangle*/
		if (p2_y == p3_y)
		{
			if (p2_x > p3_x) {
				std::swap(p2_x, p3_x);
				std::swap(p2_color, p3_color);
			}
			fillflatbottomtriangle(surface,p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d , ZBuffer , vertexnormbuffer, p1_color, p2_color, p3_color,color);
		}

		//top triangle
		else if (p1_y == p2_y) {
			if (p1_x > p2_x) {
				std::swap(p1_x, p2_x);
				std::swap(p1_color, p2_color);
			}
			fillflattoptriangle(surface,p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d,ZBuffer, vertexnormbuffer, p1_color, p2_color, p3_color, color);
		}

		else {
			int x4 = p1_x + (float)((float)(p2_y - p1_y) / (float)(p3_y - p1_y)) * (p3_x - p1_x);		//determine the oposite end of the triangle bottom/top
			
			//right side major by default perform a swap between x4 and p2_x in the functions if left side.
			//note that p2y is the middle, and it will be used for both sides).
			fillflatbottomtriangle(surface, p1_x, p1_y, p2_x, p2_y, x4, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, vertexnormbuffer, p1_color, p2_color, p3_color, color);
			fillflattoptriangle(surface, p2_x, p2_y, x4, p1_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, vertexnormbuffer, p1_color, p2_color, p3_color, color);
			
			/*
			if (x4 > p2_x) {		//right side major triangle
				fillflatbottomtriangle(surface,p1_x, p1_y, p2_x, p2_y, x4, p3_y, w, a_prime, b_prime, c_prime, d,ZBuffer, vertexnormbuffer, p1_color, p2_color, p3_color, color);
				fillflattoptriangle(surface,p2_x, p2_y, x4, p2_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d,ZBuffer, vertexnormbuffer, p1_color, p2_color, p3_color,color);
			}

			else {	//left side major triangle
				fillflatbottomtriangle(surface,p1_x, p1_y, x4, p2_y, p2_x, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, vertexnormbuffer, p1_color, p2_color,p3_color,color);	//flat bottom
				fillflattoptriangle(surface,x4, p2_y, p2_x, p2_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, vertexnormbuffer, p1_color, p2_color,p3_color,color);		//flat top
				}
				*/
		}
	}

	static void filltriangleTex(SDL_Surface* surface, SDL_Texture *texture, int u0, int v0, int u1, int v1,int u2,int v2, std::vector<float>& ZBuffer, Uint32 color = 0xFFFFFF)
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

	static void drawtriangle(SDL_Surface* surface, int x0, int y0, int x1, int y1, int x2, int y2, float w, float a_prime, float b_prime, float c_prime, float d, std::vector<float>& ZBuffer,Uint32 color = 0)
	{
		drawline(surface, x0, y0, x1, y1,  w, a_prime, b_prime, c_prime, d, ZBuffer, color);
		drawline(surface, x1, y1, x2, y2, w, a_prime, b_prime, c_prime, d, ZBuffer, color);
		drawline(surface, x0, y0, x2, y2, w, a_prime, b_prime, c_prime, d, ZBuffer, color);
									
	}

	static void drawline(SDL_Surface* surface, int x0, int y0, int x1, int y1, float w, float a_prime, float b_prime, float c_prime, float d, std::vector<float>& ZBuffer, Uint32 color = 0xFFFFFF)
	{
		for (float t = 0.; t < 1.; t += .01) {
			
			int x = x0 + (x1 - x0)*t;
			int y = y0 + (y1 - y0)*t;
			float zpos_camspace_inv = ((a_prime * x) + (b_prime * y) + c_prime);
			float zpos_ndc = zpos_camspace_inv * w;
			if (ZBuffer[x + 800 * y] > zpos_ndc) {
				ZBuffer[x + 800 * y] = zpos_ndc;
				putpixel(surface, x, y, ZBuffer, color);
			}	
		}
	}

private:
	static void fillflatbottomtriangle(SDL_Surface* surface, int p1_x, int p1_y, int p2_x, int p2_y, int p4_x, int p3_y, float w, float a_prime, float b_prime, float c_prime, float d , std::vector<float>&  ZBuffer,
		std::vector<Vector3f>& vertexnormbuffer, Uint32 p1_color, Uint32 p2_color,Uint32 p3_color,
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

		int dy = p2_y - p1_y;	
		int dx1 = p2_x - p1_x;	
		int dx2 = p4_x - p1_x;

		float slope_1 = 0; float slope_2 = 0;

		if (dy) {
			slope_1 = (float)dx1 / (float)dy;
		}

		if (dy) {
			slope_2 = (float)dx2 / (float)dy;
		}

		Uint8  Ip[4] , Ib[4], Ia[4];
			for (int scanline = p1_y; scanline <= p2_y; scanline++) {
				int px1 = slope_1 * (float)(scanline - p2_y) + p2_x;
				int px2 = slope_2 * (float)(scanline - p2_y) + p4_x;

				const int xStart = (int)px1;
				const int xEnd = (int)px2;

				//goroud calculation : https://www.youtube.com/watch?v=06p86OrTGLc&t=233s&ab_channel=raviramamoorthi
				//vertex side intensity
				//Ia = (p1_color * (scanline - p2_y) + p2_color * (p1_y - scanline)) / (p1_y - p2_y);
				if (p1_y - p2_y) {
					double t1 = (double)(scanline - p2_y) / (double)(p1_y - p2_y);
					double t2 = (double)(p1_y - scanline) / (double)(p1_y - p2_y);
					Ia[1] = ((p1_color & 0x00FF0000) >> 16) * t1 + ((p2_color & 0x00FF0000) >> 16) * t2;
					Ia[2] = ((p1_color & 0x0000FF00) >> 8) * t1 + ((p2_color & 0x0000FF00) >> 8) * t2;
					Ia[3] = (p1_color & 0x000000FF) * t1 + (p2_color & 0x000000FF) * t2;
				}

				if (p1_y - p3_y) {
					double t1 = (double)(scanline - p3_y) / (double)(p1_y - p3_y);
					double t2 = (double)(p1_y - scanline) / (double)(p1_y - p3_y);
					//Ib = (p1_color * (scanline - p3_y) + p3_color * (p1_y - scanline)) / (p1_y - p3_y);
				
					Ib[1] = ((p1_color & 0x00FF0000) >> 16) *  t1 + ((p3_color & 0x00FF0000) >> 16) * t2;
					Ib[2] = ((p1_color & 0x0000FF00) >> 8) *  t1 + ((p3_color & 0x0000FF00) >> 8) *  t2;
					Ib[3] = (p1_color & 0x000000FF) *  t1 + (p3_color & 0x000000FF) *  t2;
				}

				if (left_side_major)
				{
					std::swap(Ia, Ib);
				}

				for (int x = xStart; x <= xEnd; ++x) {

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
						Ip[0] = 0;	//alpha should always make it visible
						Ip[1] = (Ia[1] * t1) + (Ib[1] * t2);
						Ip[2] = (Ia[2] * t1) + (Ib[2] * t2);
						Ip[3] = (Ia[3] * t1) + (Ib[3] * t2);
					}

					//z buffer here
					float zpos_camspace_inv = ((a_prime * x) + (b_prime * scanline) + c_prime);
					double zpos_ndc = zpos_camspace_inv * w;
					if (ZBuffer[x + 800 * scanline] > zpos_ndc) {
						Uint32 goroudcol = (Ip[0] << 24) + (Ip[1] << 16) + (Ip[2] << 8) + Ip[3];
						ZBuffer[x + 800 * scanline] = zpos_ndc;
						putpixel(surface, x, scanline, ZBuffer, goroudcol);
					}
				}
			}
	}

	static void fillflattoptriangle(SDL_Surface* surface, int p2_x, int p2_y, int p4_x, int p1_y, int p3_x, int p3_y, float w, float a_prime, float b_prime, float c_prime, float d, std::vector<float>&  ZBuffer,  
		std::vector<Vector3f>& vertexnormbuffer, Uint32 p1_color, Uint32 p2_color,Uint32 p3_color,
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

		int dy = p3_y - p2_y;
		int dx1 = p3_x - p2_x;
		int dx2 = p3_x - p4_x;

		float slope_1 = 0; float slope_2 = 0;

		if (dy) {
			slope_1 = (float)dx1 / (float)dy;
		}

		if (dy) {
			slope_2 = (float)dx2 / (float)dy;
		}

		Uint8  Ip[4], Ib[4], Ia[4];

		for (int scanline = p3_y; scanline >= p2_y; scanline--) {
			int px1 = slope_1 * (float)(scanline - p2_y) + p2_x;
			int px2 = slope_2 * (float)(scanline - p3_y) + p3_x;

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
				//Ib = (p1_color * (scanline - p3_y) + p3_color * (p1_y - scanline)) / (p1_y - p3_y);

				Ib[1] = ((p3_color & 0x00FF0000) >> 16) *  t1 + ((p1_color & 0x00FF0000) >> 16) * t2;
				Ib[2] = ((p3_color & 0x0000FF00) >> 8) *  t1 + ((p1_color & 0x0000FF00) >> 8) *  t2;
				Ib[3] = (p3_color & 0x000000FF) *  t1 + (p1_color & 0x000000FF) *  t2;
			}

			if (left_side_major)
			{
				std::swap(Ia, Ib);
			}


			for (int x = xStart; x <= xEnd; ++x) {

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
					Ip[0] = 0;	//alpha should always make it visible
					Ip[1] = (Ia[1] * t1) + (Ib[1] * t2);
					Ip[2] = (Ia[2] * t1) + (Ib[2] * t2);
					Ip[3] = (Ia[3] * t1) + (Ib[3] * t2);
				}

				float zpos_camspace_inv = ((a_prime * x) + (b_prime * scanline) + c_prime);
				float zpos_ndc = zpos_camspace_inv * w;
				if (ZBuffer[x + 800 * scanline] > zpos_ndc) {
					Uint32 goroud = (Ip[0] << 24) + (Ip[1] << 16) + (Ip[2] << 8) + Ip[3];
					ZBuffer[x + 800 * scanline] = zpos_ndc;
					putpixel(surface, x, scanline, ZBuffer, color);
				}
			
			}
		}
	}
};