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
			if (x4 > p2_x) {		//right side major triangle
				fillflatbottomtriangle(surface,p1_x, p1_y, p2_x, p2_y, x4, p2_y, w, a_prime, b_prime, c_prime, d,ZBuffer, vertexnormbuffer, p1_color, p2_color, p3_color, color);
				fillflattoptriangle(surface,p2_x, p2_y, x4, p2_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d,ZBuffer, vertexnormbuffer, p1_color, p2_color, p3_color,color);
			}

			else {	//left side major triangle
				fillflatbottomtriangle(surface,p1_x, p1_y, x4, p2_y, p2_x, p2_y, w, a_prime, b_prime, c_prime, d, ZBuffer, vertexnormbuffer, p1_color, p2_color,p3_color,color);	//flat bottom
				fillflattoptriangle(surface,x4, p2_y, p2_x, p2_y, p3_x, p3_y, w, a_prime, b_prime, c_prime, d, ZBuffer, vertexnormbuffer, p1_color, p2_color,p3_color,color);		//flat top
			}
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
	static void fillflatbottomtriangle(SDL_Surface* surface, int x0, int y0, int x1, int y1, int x2, int y2, float w, float a_prime, float b_prime, float c_prime, float d , std::vector<float>&  ZBuffer,
		std::vector<Vector3f>& vertexnormbuffer, Uint32 p1_color, Uint32 p2_color,Uint32 p3_color,
		Uint32 color = 0xFFFFFF)
	{
		int dy1 = y1 - y0;
		int dy2 = y2 - y0;

		int dx1 = x1 - x0;
		int dx2 = x2 - x0;
		float slope_1 = 0; float slope_2 = 0;

		if (dy1) {
			slope_1 = (float)dx1 / (float)dy1;
		}

		if (dy2) {
			slope_2 = (float)dx2 / (float)dy2;
		}

		Uint32 Ia, Ib, Ip;

		for (int scanline = y0; scanline <= y1; scanline++) {
			int px1 = slope_1 * (float)(scanline - y1) + x1;	
			int px2 = slope_2 * (float)(scanline - y2) + x2;	

			const int xStart = (int)px1;
			const int xEnd = (int)px2;

			
			//goroud calcualtion : https://www.youtube.com/watch?v=06p86OrTGLc&t=233s&ab_channel=raviramamoorthi
			
			if ((y1 - y0) == 0)
			{
				Ia = p2_color;
				Ib = p3_color;
			}
			else {
				Ia = p1_color * (scanline - y0) + p2_color * (y1 - scanline) / (y1 - y0);
				Ib = p1_color * (scanline - y0) + p2_color * (y1 - scanline) / (y1 - y0);
			}
			
			for (int x = xStart; x <= xEnd; ++x) {
				
				if ((xStart - xEnd) == 0)
				{
					Ip = p1_color;
				}
				else
				{
					Ip = (Ia * (xEnd - x) + Ib * (x - xStart)) / (xEnd - xStart);
				}
				
				//z buffer here
				float zpos_camspace_inv = ((a_prime * x) + (b_prime * scanline) + c_prime);
				double zpos_ndc = zpos_camspace_inv * w;
				if (ZBuffer[x + 800 * scanline] > zpos_ndc) {
					ZBuffer[x + 800 * scanline] = zpos_ndc;
					putpixel(surface, x, scanline, ZBuffer,Ip);
				}
			}
		}
	}
	static void fillflattoptriangle(SDL_Surface* surface, int x0, int y0, int x1, int y1, int x2, int y2, float w, float a_prime, float b_prime, float c_prime, float d, std::vector<float>&  ZBuffer,  
		std::vector<Vector3f>& vertexnormbuffer, Uint32 p1_color, Uint32 p2_color,Uint32 p3_color,
		Uint32 color = 0xFFFFFF)
	{
		int dy1 = y2 - y0;
		int dy2 = y2 - y1;

		int dx1 = x2 - x0;
		int dx2 = x2 - x1;
		float slope_1 = 0; float slope_2 = 0;

		if (dy1) {
			slope_1 = (float)dx1 / (float)dy1;
		}


		if (dy2) {
			slope_2 = (float)dx2 / (float)dy2;
		}

		
		for (int scanline = y2; scanline >= y0; scanline--) {
			int px1 = slope_1 * (float)(scanline - y0) + x0;
			int px2 = slope_2 * (float)(scanline - y2) + x2;


			const int xstart = (int)px1;
			const int xEnd = (int)px2;

			for (int x = xstart; x <= xEnd; ++x) {
				float zpos_camspace_inv = ((a_prime * x) + (b_prime * scanline) + c_prime);
				float zpos_ndc = zpos_camspace_inv * w;
				if (ZBuffer[x + 800 * scanline] > zpos_ndc) {
					ZBuffer[x + 800 * scanline] = zpos_ndc;
					putpixel(surface, x, scanline, ZBuffer, color);
				}
			
			}
		}
	}
};