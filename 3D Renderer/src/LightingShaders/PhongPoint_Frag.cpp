#include "PhongPoint_Frag.h"

void PhongPoint_Frag::filltriangle_phong_point(SDL_Surface * surface, double p1_x, double p1_y, double p1_z, double p1_w, double p2_x, double p2_y, double p2_z, double p2_w, double p3_x, double p3_y, double p3_z, double p3_w, Vector3f & camerapos, std::vector<double>& ZBuffer, Mat3f Mat, Vector3f v1_vertex, Vector3f v2_vertex, Vector3f v3_vertex, PointLightSetup & pl, Uint32 color)
{
	if (p2_y < p1_y) {
		std::swap(p2_y, p1_y);
		std::swap(p2_x, p1_x);
		std::swap(p2_z, p1_z);
		std::swap(p2_w, p1_w);
		std::swap(v2_vertex, v1_vertex);
	}

	if (p3_y < p1_y) {
		std::swap(p3_y, p1_y);
		std::swap(p3_x, p1_x);
		std::swap(p3_z, p1_z);
		std::swap(p3_w, p1_w);
		std::swap(v3_vertex, v1_vertex);
	}

	if (p3_y < p2_y) {
		std::swap(p3_y, p2_y);
		std::swap(p3_x, p2_x);
		std::swap(p3_z, p2_z);
		std::swap(p3_w, p2_w);
		std::swap(v3_vertex, v2_vertex);
	}

	if (p1_y == p2_y)
	{
		fillflattoptriangle_phong_point(surface, p1_x, p1_y, p1_z, p1_w, p2_x, p2_y, p2_z, p2_w, p3_x, p3_y, p3_z, p3_w, ZBuffer, Mat, camerapos, v1_vertex, v2_vertex, v3_vertex, pl, color);
	}

	else if (p2_y == p3_y)
	{
		fillflatbottomtriangle_phong_point(surface, p1_x, p1_y, p1_z, p1_w, p2_x, p2_y, p2_z, p2_w, p3_x, p3_y, p3_z, p3_w, ZBuffer, Mat, camerapos, v1_vertex, v2_vertex, v3_vertex, pl, color);
	}

	else {

		double x4 = (double)p1_x + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (double)(p3_x - p1_x);		//determine the oposite end of the triangle bottom/top
		double z4 = (double)p1_z + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (double)(p3_z - p1_z);		//determine the oposite end of the triangle bottom/top
		double w4 = (double)p1_w + (double)((double)(p2_y - p1_y) / (double)(p3_y - p1_y)) * (double)(p3_w - p1_w);		//determine the oposite end of the triangle bottom/top
		double t = (double)(p3_y - p2_y) / (double)(p3_y - p1_y);

		Vector3f v4_vertex = v3_vertex * (1.0 - t) + v1_vertex * t;
		v4_vertex.Normalize();

		//right side major by default perform a swap between x4 and p2_x in the functions if left side.
		//note that p2y is the middle, and it will be used for both sides).
		fillflatbottomtriangle_phong_point(surface, p1_x, p1_y, p1_z, p1_w, p2_x, p2_y, p2_z, p2_w, x4, p3_y, z4, w4, ZBuffer, Mat, camerapos, v1_vertex, v2_vertex, v4_vertex, pl, color);
		fillflattoptriangle_phong_point(surface, p2_x, p2_y, p2_z, p2_w, x4, p1_y, z4, w4, p3_x, p3_y, p3_z, p3_w, ZBuffer, Mat, camerapos, v4_vertex, v2_vertex, v3_vertex, pl, color);
	}
}

void PhongPoint_Frag::fillflatbottomtriangle_phong_point(SDL_Surface * surface, double p1_x, double p1_y, double p1_z, double p1_w, double p2_x, double p2_y, double p2_z, double p2_w, double p4_x, double p3_y, double p4_z, double p4_w, std::vector<double>& ZBuffer, Mat3f & Mat, Vector3f & camerapos, Vector3f p1_vertex, Vector3f p2_vertex, Vector3f p4_vertex, PointLightSetup & pl, Uint32 color)
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
		std::swap(p4_z, p2_z);
		std::swap(p4_vertex, p2_vertex);
		std::swap(p4_w, p2_w);
	}

	double dy = p2_y - p1_y;
	double dx1 = p2_x - p1_x;
	double dx2 = p4_x - p1_x;
	double dz1 = p2_z - p1_z;
	double dz2 = p4_z - p1_z;
	double dw1 = p2_w - p1_w;
	double dw2 = p4_w - p1_w;

	double slope_1 = 0; double slope_2 = 0;
	double slope_1_z = 0; double slope_2_z = 0;
	double slope_1_w = 0; double slope_2_w = 0;

	if (dy) {
		slope_1 = dx1 / dy;
		slope_1_z = dz1 / dy;
		slope_1_w = dw1 / dy;
	}

	if (dy) {
		slope_2 = dx2 / dy;
		slope_2_z = dz2 / dy;
		slope_2_w = dw2 / dy;
	}

	Vector3f Va, Vb, Vp;

	Uint8 light_src_rgba[4], final_light_rgba[4];
	Uint32 final_light;
	light_src_rgba[0] = (color & 0xFF000000) >> 24;
	light_src_rgba[1] = (color & 0x00FF0000) >> 16;
	light_src_rgba[2] = (color & 0x0000FF00) >> 8;
	light_src_rgba[3] = (color & 0x000000FF);
	final_light_rgba[0] = light_src_rgba[0];

	int yStart = p1_y;
	int yEnd = p2_y;

	if (yStart <= 1) { yStart = 1; }
	if (yEnd >= 599) { yEnd = 599; }

	for (int scanline = yStart; scanline <= yEnd; scanline++) {
		double px1 = slope_1 * ((double)scanline - p2_y) + p2_x;
		double px2 = slope_2 * ((double)scanline - p2_y) + p4_x;

		double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
		double zEnd = slope_2_z * ((double)scanline - p2_y) + p4_z;

		double wStart = slope_1_w * ((double)scanline - p2_y) + p2_w;
		double wEnd = slope_2_w * ((double)scanline - p2_y) + p4_w;

		int xStart = (int)px1;
		int xEnd = (int)px2;

		if (xStart <= 1) { xStart = 1; }
		if (xEnd >= 799) { xEnd = 799; }

		double t = ((double)scanline - p2_y) / (double)(p1_y - p2_y);

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

				Vp.Normalize();

				//convert frag coords to world
				Vector3f view(x, scanline, z_frag);	//screen space
				view = Mat3f::Scale(1 / (0.5 * SCREENWIDTH), 1 / (0.5 * SCREENHEIGHT), 1) * view;
				view = Mat3f::Translate(-1, -1, 0) * view;
				float w_frag = wStart * t_x + wEnd * (1.0 - t_x);
				view.x /= w_frag;
				view.y /= w_frag;
				view.z /= w_frag;
				view = Mat * view;

				//attentuation
				Vector3f to_light = pl.lightpos - view;
				double dist = to_light.getMagnitude();	//get distance from point lgiht to vertex point
				double attenuation = 1.0 / ((pl.a * dist * dist) + (pl.b * dist) + pl.c);	//get attenuation
				to_light.Normalize();

				//ambient
				double amb_k = pl.amb_constant;

				//diffuse
				double diff_k = std::max(0.0f, to_light.getDotProduct(Vp));

				//specular
				Vector3f ViewVec = camerapos - view;
				Vector3f w = Vp * 2.0 * Vp.getDotProduct(to_light);
				Vector3f r = w - to_light;
				double spec_k = std::max(0.0f, std::powf((ViewVec.getNormalized().getDotProduct(r.getNormalized())), pl.spec_exponent));

				if (spec_k > 1.0f)
				{
					spec_k = 1.0f;
				}

				double f = (amb_k + attenuation * (diff_k + (spec_k * pl.spec_intensity)));

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

void PhongPoint_Frag::fillflattoptriangle_phong_point(SDL_Surface * surface, double p2_x, double p2_y, double p2_z, double p2_w, double p4_x, double p1_y, double p4_z, double p4_w, double p3_x, double p3_y, double p3_z, double p3_w, std::vector<double>& ZBuffer, Mat3f & Mat, Vector3f & camerapos, Vector3f p4_vertex, Vector3f p2_vertex, Vector3f p3_vertex, PointLightSetup & pl, Uint32 color)
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
		std::swap(p2_w, p4_w);
		std::swap(p2_vertex, p4_vertex);
	}

	double dy = p3_y - p2_y;
	double dx1 = p3_x - p2_x;
	double dx2 = p3_x - p4_x;
	double dz1 = p3_z - p2_z;
	double dz2 = p3_z - p4_z;
	double dw1 = p3_w - p2_w;
	double dw2 = p3_w - p4_w;

	double slope_1 = 0; double slope_2 = 0;
	double slope_1_z = 0; double slope_2_z = 0;
	double slope_1_w = 0; double slope_2_w = 0;

	if (dy) {
		slope_1 = dx1 / dy;
		slope_1_z = dz1 / dy;
		slope_1_w = dw1 / dy;
	}

	if (dy) {
		slope_2 = dx2 / dy;
		slope_2_z = dz2 / dy;
		slope_2_w = dw2 / dy;
	}

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

	if (yStart >= 599) { yStart = 599; }
	if (yEnd <= 1) { yEnd = 1; }

	for (int scanline = yStart; scanline > yEnd; scanline--) {
		double px1 = slope_1 * ((double)scanline - p2_y) + p2_x;
		double px2 = slope_2 * ((double)scanline - p3_y) + p3_x;

		double zStart = slope_1_z * ((double)scanline - p2_y) + p2_z;
		double zEnd = slope_2_z * ((double)scanline - p3_y) + p3_z;

		double wStart = slope_1_w * ((double)scanline - p2_y) + p2_w;
		double wEnd = slope_2_w * ((double)scanline - p3_y) + p3_w;

		int xStart = (int)px1;
		int xEnd = (int)px2;

		if (xStart <= 1) { xStart = 1; }
		if (xEnd >= SCREENWIDTH - 1) { xEnd = SCREENWIDTH - 1; }

		double t = ((double)scanline - p2_y) / (double)(p3_y - p2_y);

		//LHS
		Va = p3_vertex * t + p2_vertex * (1.0 - t);
		Va.Normalize();

		//RHS
		Vb = p3_vertex * t + p4_vertex * (1.0 - t);
		Vb.Normalize();

		for (int x = xStart; x <= xEnd; ++x) {

			double t_x = (double)(xEnd - x) / (double)(xEnd - xStart);
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

				Vp.Normalize();

				//screen coord -> viewspace coord
				Vector3f view(x, scanline, z_frag);
				view = Mat3f::Scale(1 / (0.5 * SCREENWIDTH), 1 / (0.5 * SCREENHEIGHT), 1) * view;
				view = Mat3f::Translate(-1, -1, 0) * view;
				float w_frag = wStart * t_x + wEnd * (1.0 - t_x);

				view.x /= w_frag;
				view.y /= w_frag;
				view.z /= w_frag;

				view = Mat * view;

				//gotta invert the view mat
				Vector3f to_light = pl.lightpos - view;
				double dist = to_light.getMagnitude();	//get distance from point lgiht to vertex point
				double attenuation = 1.0 / ((pl.a * dist * dist) + (pl.b * dist) + pl.c);	//get attenuation
				to_light.Normalize();	//normalize

				//ambient
				double amb_k = pl.amb_constant;

				//diffuse
				double diff_k = std::max(0.0f, to_light.getDotProduct(Vp));

				//specular
				Vector3f ViewVec = camerapos - view;
				Vector3f w = Vp * 2.0 * Vp.getDotProduct(to_light);
				Vector3f r = w - to_light;
				double spec_k = std::max(0.0f, std::powf((ViewVec.getNormalized().getDotProduct(r.getNormalized())), pl.spec_exponent));

				if (spec_k > 1.0)
				{
					spec_k = 1.0;
				}

				double f = (amb_k + attenuation * (diff_k + (spec_k * pl.spec_intensity)));

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