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
	static void filltriangle(SDL_Surface* surface, int x0, int y0, int x1, int y1, int x2, int y2,float w, float a_prime, float b_prime, float c_prime, float d ,std::vector<float>& ZBuffer, Uint32 color = 0xFFFFFF)
	{
		if (y1 < y0) {
			std::swap(y1, y0);
			std::swap(x1, x0);
		}

		if (y2 < y0) {
			std::swap(y2, y0);
			std::swap(x2, x0);
		}

		if (y2 < y1) {
			std::swap(y2, y1);
			std::swap(x2, x1);
		}

		/*if it is only a bottom flat triangle*/
		if (y1 == y2)
		{
			if (x1 > x2) {
				std::swap(x1, x2);
			}
			fillbottomtriangle(surface,x0, y0, x1, y1, x2, y2, w, a_prime, b_prime, c_prime, d , ZBuffer , color);
		}

		else if (y0 == y1) {
			if (x0 > x1) {
				std::swap(x0, x1);
			}
			filltoptriangle(surface,x0, y0, x1, y1, x2, y2, w, a_prime, b_prime, c_prime, d,ZBuffer, color);
		}

		else {
			int x4 = x0 + (float)((float)(y1 - y0) / (float)(y2 - y0)) * (x2 - x0);
			if (x4 > x1) {
				fillbottomtriangle(surface,x0, y0, x1, y1, x4, y1, w, a_prime, b_prime, c_prime, d,ZBuffer, color);
				filltoptriangle(surface,x1, y1, x4, y1, x2, y2, w, a_prime, b_prime, c_prime, d,ZBuffer, color);
			}
			else {
				fillbottomtriangle(surface,x0, y0, x4, y1, x1, y1, w, a_prime, b_prime, c_prime, d, ZBuffer, color);
				filltoptriangle(surface,x4, y1, x1, y1, x2, y2, w, a_prime, b_prime, c_prime, d, ZBuffer, color);
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
	static void fillbottomtriangle(SDL_Surface* surface, int x0, int y0, int x1, int y1, int x2, int y2, float w, float a_prime, float b_prime, float c_prime, float d , std::vector<float>&  ZBuffer, Uint32 color = 0xFFFFFF)
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


		for (int scanline = y0; scanline <= y1; scanline++) {
			int px1 = slope_1 * (float)(scanline - y1) + x1;	
			int px2 = slope_2 * (float)(scanline - y2) + x2;	


			const int xstart = (int)px1;
			const int xEnd = (int)px2;

			for (int x = xstart; x <= xEnd; ++x) {

				//z buffer here
				float zpos_camspace_inv = ((a_prime * x) + (b_prime * scanline) + c_prime);
				double zpos_ndc = zpos_camspace_inv * w;
				if (ZBuffer[x + 800 * scanline] > zpos_ndc) {
					ZBuffer[x + 800 * scanline] = zpos_ndc;
					putpixel(surface, x, scanline, ZBuffer, color);
				}
			}
		}
	}
	static void filltoptriangle(SDL_Surface* surface, int x0, int y0, int x1, int y1, int x2, int y2, float w, float a_prime, float b_prime, float c_prime, float d, std::vector<float>&  ZBuffer,  Uint32 color = 0xFFFFFF)
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
				float zpos_camspace = 1 / ((a_prime * x) + (b_prime * scanline) + c_prime);
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