#pragma once
#include "Vector3.h"

template <typename T>
class Mat3 {
public:
	Vector3<T> operator*(const Vector3<T>& other) 		//ouputs the 3 by 1 matrix i.e. vector
	{
		Vector3<T> output;
		output.x = other.x * cells[0][0] + other.y * cells[0][1] + other.z * cells[0][2] + cells[0][3];
		output.y = other.x * cells[1][0] + other.y * cells[1][1] + other.z * cells[1][2] + cells[1][3];
		output.z = other.x * cells[2][0] + other.y * cells[2][1] + other.z * cells[2][2] + cells[2][3];
		T w = other.x * cells[3][0] + other.y * cells[3][1] + other.z * cells[3][2] + cells[3][3];
		this->w = w;
		return output;
	}

	Mat3<T> operator*(const Mat3& other) const		//multiply with aother matrix
	{
		Mat3<T> out;
		for (int row_left = 0; row_left < 4; row_left++)
		{
			for (int col_right = 0; col_right < 4; col_right++)
			{
				out.cells[row_left][col_right] = 0;
				for (int i = 0; i < 4; i++)
				{
					out.cells[row_left][col_right] += cells[row_left][i] * other.cells[i][col_right];
				}
			}
		}
		return out;
	}

	static Mat3<T> Scale(T factorX, T factorY, T factorZ) 
	{
		Mat3<T> Scale;

		Scale.cells[0][0] = factorX;
		Scale.cells[1][1] = factorY;
		Scale.cells[2][2] = factorZ;
		Scale.cells[3][3] = 1;
		return Scale;
	}

	static Mat3<T> Identity()	{
		Mat3<T> identity;
		for (int i = 0; i < 4; ++i) {		//only 0,1 and 2, as 3 is part of w.
			identity.cells[i][i] = 1.0f;
		}
		return identity;
	}

	static Mat3<T> RotateX(T theta) 
	{
		Mat3<T> matRotX;
		matRotX.cells[0][0] = 1;
		matRotX.cells[1][1] = cosf(theta);
		matRotX.cells[1][2] = -sinf(theta);
		matRotX.cells[2][1] = sinf(theta);
		matRotX.cells[2][2] = cosf(theta);
		matRotX.cells[3][3] = 1;
		return matRotX;
	}

	static Mat3<T> RotateY(T theta)
	{
		Mat3<T> matRotY;
		matRotY.cells[0][0] = cosf(theta);
		matRotY.cells[0][2] = sinf(theta);
		matRotY.cells[2][0] = -sinf(theta);
		matRotY.cells[1][1] = 1;
		matRotY.cells[2][2] = cosf(theta);
		matRotY.cells[3][3] = 1;
		return matRotY;
	}

	static Mat3<T> RotateZ(T theta)
	{
		Mat3<T> matRotZ;
		matRotZ.cells[0][0] = cosf(theta);
		matRotZ.cells[1][0] = sinf(theta);
		matRotZ.cells[0][1] = -sinf(theta);
		matRotZ.cells[1][1] = cosf(theta);
		matRotZ.cells[2][2] = 1;
		matRotZ.cells[3][3] = 1;
		return matRotZ;
	}

	static Mat3<T> Translate (T x, T y, T z){			//translation.
		Mat3<T> Translation;
		for (int i = 0; i < 4; ++i) {		//only 0,1 and 2, as 3 is part of w.
			Translation.cells[i][i] = 1;
		}
		
		Translation.cells[0][3] = x;
		Translation.cells[1][3] = y;
		Translation.cells[2][3] = z;
	
		return Translation;
	}

	static Mat3<T> Projection(T Near, T Far, T AspectRatio, T FOV) {			//projectio matrix
		Mat3<T> Projection;
		float Deg2Rad = 3.14159f / 180.0f;
		
		T FovRad = 1.0f / tanf((FOV * 0.5f) * Deg2Rad);
		Projection.cells[0][0] = (1.0/AspectRatio) * FovRad;
		Projection.cells[1][1] = FovRad;
		Projection.cells[2][2] = Far / (Far - Near);
		Projection.cells[2][3] = (-Far * Near) / (Far - Near);
		Projection.cells[3][2] = 1.0f;
		Projection.cells[3][3] = 0.0f;


		
		//T t = tanf(FOV * 0.5 * Deg2Rad) * Near;
		//T b = -t;
		//T r = tanf(FOV * 0.5 * Deg2Rad) * Near * AspectRatio;
		//T l = -tanf(FOV * 0.5 * Deg2Rad) * Near * AspectRatio;
		//
		//Projection.cells[0][0] = 2 * Near / (r - l);
		//Projection.cells[0][2] = ( r + l )/ (r - l);
		//
		//Projection.cells[1][1] = ( 2 * Near )/ (t - b);
		//Projection.cells[1][2] = (t + b) / (t - b);
		//
		//Projection.cells[2][2] = -(Far + Near) / (Far - Near);
		//Projection.cells[2][3] = (2 * Far * Near) / (Far - Near);
		//Projection.cells[3][2] = -1.0f;
		


		return Projection;
	}

	static Mat3<T> CameraPointAt(Vector3<T>& camerapos, Vector3<T>& LookDir){
		/*matrix parameter calculations*/
		Vector3<T> Forward = (LookDir).getNormalized();			//pos = camera positon, target = object target (the vector in which the caemra is pointing to).

		const Vector3<T> tmp(0.0, 1.0f, 0.0f);		//if forward = (0,0,1), tmp = (0,1,0), then ortho should be (1,0,0)
		
		Vector3<T> Right = Forward.getCrossProduct(tmp);

		Vector3<T> Up = Forward.getCrossProduct(Right);

		/*matrix setup*/
		Mat3<T> CameraPointTo;
		CameraPointTo.cells[0][0] = Right.x;
		CameraPointTo.cells[0][1] = Up.x;
		CameraPointTo.cells[0][2] = Forward.x;
		CameraPointTo.cells[0][3] = camerapos.x;

		CameraPointTo.cells[1][0] = Right.y;
		CameraPointTo.cells[1][1] = Up.y;
		CameraPointTo.cells[1][2] = Forward.y;
		CameraPointTo.cells[1][3] = camerapos.y;

		CameraPointTo.cells[2][0] = Right.z;
		CameraPointTo.cells[2][1] = Up.z;
		CameraPointTo.cells[2][2] = Forward.z;
		CameraPointTo.cells[2][3] = camerapos.z;

		CameraPointTo.cells[3][3] = 1.0f;

		return CameraPointTo;
	}

	static Mat3<T> InverseCamera(const Mat3<T>& other) {
		Mat3<T> CameraInverse;

		Vector3<T> CameraVec(other.cells[0][3], other.cells[1][3], other.cells[2][3]);
		Vector3<T> OrthoVec(other.cells[0][0], other.cells[1][0], other.cells[2][0]);
		Vector3<T> UpVec(other.cells[0][1], other.cells[1][1], other.cells[2][1]);
		Vector3<T> ForwardVec(other.cells[0][2], other.cells[1][2], other.cells[2][2]);

		CameraInverse.cells[0][0] = other.cells[0][0];
		CameraInverse.cells[0][1] = other.cells[1][0];
		CameraInverse.cells[0][2] = other.cells[2][0];
		CameraInverse.cells[0][3] = -CameraVec.getDotProduct(OrthoVec);

		CameraInverse.cells[1][0] = other.cells[0][1];
		CameraInverse.cells[1][1] = other.cells[1][1];
		CameraInverse.cells[1][2] = other.cells[2][1];
		CameraInverse.cells[1][3] = -CameraVec.getDotProduct(UpVec);

		CameraInverse.cells[2][0] = other.cells[0][2];
		CameraInverse.cells[2][1] = other.cells[1][2];
		CameraInverse.cells[2][2] = other.cells[2][2];
		CameraInverse.cells[2][3] = -CameraVec.getDotProduct(ForwardVec);

		CameraInverse.cells[3][3] = 1.0f;
		
		return CameraInverse;
	}
	
	static Vector3<T> Normalize (const Vector3<T>& other,const Mat3<T>& MatProj)			//normalize the output vector/matrix.
	{
		if (MatProj.w != 0.0f)
		{
			return Vector3<T>(other.x/ MatProj.w, other.y / MatProj.w, other.z / MatProj.w);
		}
		return other;
	}

	static Mat3<T> Inverse(const Mat3<T>& a)
	{
		Mat3<T> Inv;
		
		//implementation: https://semath.info/src/inverse-cofactor-ex4.html
		T det = (a.cells[0][0] * a.cells[1][1] * a.cells[2][2] * a.cells[3][3]) + (a.cells[0][0] * a.cells[1][2] * a.cells[2][3] * a.cells[3][1]) + (a.cells[0][0] * a.cells[1][3] * a.cells[2][1] * a.cells[3][2])
			- (a.cells[0][0] * a.cells[1][3] * a.cells[2][2] * a.cells[3][1]) - (a.cells[0][0] * a.cells[1][2] * a.cells[2][1] * a.cells[3][3]) - (a.cells[0][0] * a.cells[1][1] * a.cells[2][3] * a.cells[3][2])
			- (a.cells[0][1] * a.cells[1][0] * a.cells[2][2] * a.cells[3][3]) - (a.cells[0][2] * a.cells[1][0] * a.cells[2][3] * a.cells[3][1]) - (a.cells[0][3] * a.cells[1][0] * a.cells[2][1] * a.cells[3][2])
			+ (a.cells[0][3] * a.cells[1][0] * a.cells[2][2] * a.cells[3][1]) + (a.cells[0][2] * a.cells[1][0] * a.cells[2][1] * a.cells[3][3]) + (a.cells[0][1] * a.cells[1][0] * a.cells[2][3] * a.cells[3][2])
			+ (a.cells[0][1] * a.cells[1][2] * a.cells[2][0] * a.cells[3][3]) + (a.cells[0][2] * a.cells[1][3] * a.cells[2][0] * a.cells[3][1]) + (a.cells[0][3] * a.cells[1][1] * a.cells[2][0] * a.cells[3][2])
			- (a.cells[0][3] * a.cells[1][2] * a.cells[2][0] * a.cells[3][1]) - (a.cells[0][2] * a.cells[1][1] * a.cells[2][0] * a.cells[3][3]) - (a.cells[0][1] * a.cells[1][3] * a.cells[2][0] * a.cells[3][2])
			- (a.cells[0][1] * a.cells[1][2] * a.cells[2][3] * a.cells[3][0]) - (a.cells[0][2] * a.cells[1][3] * a.cells[2][0] * a.cells[3][0]) - (a.cells[0][3] * a.cells[1][1] * a.cells[2][2] * a.cells[3][0])
			+ (a.cells[0][3] * a.cells[1][2] * a.cells[2][1] * a.cells[3][0]) + (a.cells[0][2] * a.cells[1][1] * a.cells[2][3] * a.cells[3][0]) + (a.cells[0][1] * a.cells[1][3] * a.cells[2][2] * a.cells[3][0]);

		
		T inv_det = std::fabs(det);
		inv_det = 1.0 / inv_det;

		Inv.cells[0][0] = (a.cells[1][1] * a.cells[2][2] + a.cells[3][3]) + (a.cells[1][2] * a.cells[2][3] * a.cells[3][1]) + (a.cells[1][3] * a.cells[2][1] * a.cells[3][2])
					- (a.cells[1][3] * a.cells[2][2] * a.cells[3][1]) - (a.cells[1][2] * a.cells[2][1] * a.cells[3][3]) - (a.cells[1][1] * a.cells[2][3] * a.cells[3][2]);
		Inv.cells[0][0] = Inv.cells[0][0] / 2;

		Inv.cells[0][1] = -(a.cells[0][1] * a.cells[2][2] * a.cells[3][3]) - (a.cells[0][2] * a.cells[2][3] * a.cells[3][1]) - (a.cells[0][3] * a.cells[2][1] * a.cells[3][2])
					+ (a.cells[0][3] * a.cells[2][2] * a.cells[3][1]) + (a.cells[0][2] * a.cells[2][1] * a.cells[3][3]) + (a.cells[0][1] * a.cells[2][3] * a.cells[3][2]);
		
		Inv.cells[0][2] = (a.cells[0][1] * a.cells[1][2] * a.cells[3][3]) + (a.cells[0][2] * a.cells[1][3] * a.cells[3][1]) + (a.cells[0][3] * a.cells[1][1] * a.cells[3][2])
					- (a.cells[0][3] * a.cells[2][2] * a.cells[3][1]) - (a.cells[0][2] * a.cells[2][1] * a.cells[3][3]) - (a.cells[0][1] * a.cells[2][3] * a.cells[3][2]);

		Inv.cells[0][3] = -(a.cells[0][1] * a.cells[1][2] * a.cells[2][3]) - (a.cells[0][2] * a.cells[1][3] * a.cells[2][1]) - (a.cells[0][3] * a.cells[1][1] * a.cells[2][2])
					+ (a.cells[0][3] * a.cells[1][2] * a.cells[2][1]) + (a.cells[0][2] * a.cells[1][1] * a.cells[2][3]) - (a.cells[0][1] * a.cells[1][3] * a.cells[2][2]);
		
		
		
		Inv.cells[1][0] = -(a.cells[1][0] * a.cells[2][2] + a.cells[3][3]) - (a.cells[1][2] * a.cells[2][3] * a.cells[3][0]) - (a.cells[1][3] * a.cells[2][0] * a.cells[3][2])
					+ (a.cells[1][3] * a.cells[2][2] * a.cells[3][1]) + (a.cells[1][2] * a.cells[2][0] * a.cells[3][3]) + (a.cells[1][0] * a.cells[2][3] * a.cells[3][2]);

		Inv.cells[1][1] = (a.cells[0][0] * a.cells[2][2] * a.cells[3][3]) + (a.cells[0][2] * a.cells[2][3] * a.cells[3][0]) + (a.cells[0][3] * a.cells[2][0] * a.cells[3][2])
					- (a.cells[0][3] * a.cells[2][2] * a.cells[3][0]) - (a.cells[0][2] * a.cells[2][0] * a.cells[3][3]) - (a.cells[0][0] * a.cells[2][3] * a.cells[3][2]);

		Inv.cells[1][2] = -(a.cells[0][0] * a.cells[1][2] * a.cells[3][3]) - (a.cells[0][2] * a.cells[1][3] * a.cells[3][0]) - (a.cells[0][3] * a.cells[1][0] * a.cells[3][2])
					+ (a.cells[0][3] * a.cells[1][2] * a.cells[3][0]) + (a.cells[0][2] * a.cells[2][0] * a.cells[3][3]) + (a.cells[0][0] * a.cells[1][3] * a.cells[3][2]);

		Inv.cells[1][3] = (a.cells[0][0] * a.cells[1][2] * a.cells[2][3]) + (a.cells[0][2] * a.cells[1][3] * a.cells[2][0]) + (a.cells[0][3] * a.cells[1][0] * a.cells[2][2])
					- (a.cells[0][3] * a.cells[1][2] * a.cells[2][0]) - (a.cells[0][2] * a.cells[1][0] * a.cells[2][3]) - (a.cells[0][0] * a.cells[1][3] * a.cells[2][2]);


		
		Inv.cells[2][0] = (a.cells[1][0] * a.cells[2][1] + a.cells[3][3]) + (a.cells[1][1] * a.cells[2][3] * a.cells[3][0]) + (a.cells[1][3] * a.cells[2][0] * a.cells[3][1])
					- (a.cells[1][3] * a.cells[2][1] * a.cells[3][0]) - (a.cells[1][1] * a.cells[2][0] * a.cells[3][3]) - (a.cells[1][0] * a.cells[2][3] * a.cells[3][1]);
			
		Inv.cells[2][1] = -(a.cells[0][0] * a.cells[2][1] * a.cells[3][3]) - (a.cells[0][1] * a.cells[2][3] * a.cells[3][0]) - (a.cells[0][3] * a.cells[2][0] * a.cells[3][1]) 
			+ (a.cells[0][3] * a.cells[2][1] * a.cells[3][0]) + (a.cells[0][1] * a.cells[2][0] * a.cells[3][3]) + (a.cells[0][0] * a.cells[2][3] * a.cells[3][1]);

		Inv.cells[2][2] = (a.cells[0][0] * a.cells[1][1] * a.cells[3][3]) + (a.cells[0][1] * a.cells[1][3] * a.cells[3][0]) + (a.cells[0][3] * a.cells[1][0] * a.cells[3][1])
					- (a.cells[0][3] * a.cells[1][1] * a.cells[3][0]) - (a.cells[0][1] * a.cells[1][0] * a.cells[3][3]) - (a.cells[0][0] * a.cells[1][3] * a.cells[3][1]);

		Inv.cells[2][3] = -(a.cells[0][0] * a.cells[1][1] * a.cells[2][3]) - (a.cells[0][1] * a.cells[1][3] * a.cells[2][0]) - (a.cells[0][3] * a.cells[1][0] * a.cells[2][1])
					+ (a.cells[0][3] * a.cells[1][1] * a.cells[2][0]) + (a.cells[0][1] * a.cells[1][0] * a.cells[2][3]) - (a.cells[0][0] * a.cells[1][3] * a.cells[2][1]);
	
		

		Inv.cells[3][0] = -(a.cells[1][0] * a.cells[2][1] + a.cells[3][2]) - (a.cells[1][1] * a.cells[2][2] * a.cells[3][0]) - (a.cells[1][2] * a.cells[2][0] * a.cells[3][1])
					+ (a.cells[1][2] * a.cells[2][1] * a.cells[3][0]) + (a.cells[1][1] * a.cells[2][0] * a.cells[3][2]) + (a.cells[1][0] * a.cells[2][2] * a.cells[3][1]);

		Inv.cells[3][1] = (a.cells[0][0] * a.cells[2][1] * a.cells[3][2]) + (a.cells[0][1] * a.cells[2][2] * a.cells[3][0]) + (a.cells[0][2] * a.cells[2][0] * a.cells[3][1])
					- (a.cells[0][2] * a.cells[2][1] * a.cells[3][0]) - (a.cells[0][1] * a.cells[2][0] * a.cells[3][2]) - (a.cells[0][0] * a.cells[2][2] * a.cells[3][1]);

		Inv.cells[3][2] = -(a.cells[0][0] * a.cells[1][1] * a.cells[3][2]) - (a.cells[0][1] * a.cells[1][2] * a.cells[3][0]) - (a.cells[0][2] * a.cells[1][0] * a.cells[3][1])
					+ (a.cells[0][2] * a.cells[1][1] * a.cells[3][0]) + (a.cells[0][1] * a.cells[1][0] * a.cells[3][2]) + (a.cells[0][0] * a.cells[1][2] * a.cells[3][1]);

		Inv.cells[3][3] = (a.cells[0][0] * a.cells[1][1] * a.cells[2][2]) + (a.cells[0][1] * a.cells[1][2] * a.cells[2][0]) + (a.cells[0][2] * a.cells[1][0] * a.cells[2][1])
					- (a.cells[0][2] * a.cells[1][1] * a.cells[2][0]) - (a.cells[0][1] * a.cells[1][0] * a.cells[2][2]) - (a.cells[0][0] * a.cells[1][2] * a.cells[2][1]);

		for (int r = 0; r < 4; ++r)
		{
			for (int c = 0; c < 4; ++c)
			{
				Inv.cells[r][c] = inv_det * Inv.cells[r][c];
			}
		}
	
		return Inv;
	}

	T w;
private:
	T cells[4][4] = { 0.0f };		//[r][c]
	
};

using Mat3f = Mat3<float>;
using Mat3d = Mat3<double>;