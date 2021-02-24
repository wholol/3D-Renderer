#pragma once
#include "SDL.h"
#include "Vector3.h"
//FRAGMENT PART
#include <algorithm>
#include <iostream>
#include "LightSetup.h"
#include <vector>
#include "src/Core/ScreenSize.h"

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

	static void drawtriangle(SDL_Surface* surface, double x0, double y0, double x1, double y1, double x2, double y2,Uint32 color = 0)
	{
		drawline(surface, x0, y0, x1, y1, color);
		drawline(surface, x1, y1, x2, y2, color);
		drawline(surface, x0, y0, x2, y2, color);
	}

	static void drawline(SDL_Surface* surface, double x0, double y0, double x1, double y1, Uint32 color = 0xFFFFFF)
	{
		if (x0 < 0)
		{
			return;
		}
		if (x0 > SCREENWIDTH)
		{
			return;
		}
		if (y0 < 0)
		{
			return;
		}
		if (y0 > SCREENHEIGHT)
		{
			return;
		}
		
		for (double t = 0; t <= 1.0; t += .01) {
			double x = x0 + (x1 - x0)*t;
			double y = y0 + (y1 - y0)*t;

			if (x < 0)	{}
		
			else if (x > SCREENWIDTH) {}
			
			else if (y < 0)	{}

			else if (y > SCREENHEIGHT) {}
			
			else {
				putpixel(surface, x, y, color);
			}
			
		}
	}
};