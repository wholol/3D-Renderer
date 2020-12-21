#pragma once
#include <sstream>
//operator overlaoded vector class
template <typename T>
class Vector3 {
public:
	T x, y, z;			//equivalent to i,j,k.

	
	Vector3() : x(0), y(0), z(0) {}
	Vector3(T x, T y ,T z) : x(x), y(y), z(z) {}
	Vector3& operator ()(T x, T y, T z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		return *this;
	}
	Vector3& operator +=(const Vector3& other) 
	{ 
		this->x += other.x; this->y += other.y; this->z += other.z; return *this;
	}

	Vector3& operator -=(const Vector3& other) { 
		this->x -= other.x; this->y -= other.y; this->z -= other.z; return *this;
	}

	Vector3& operator *=(const T& other) { 
		this->x *= other.x; this->y *= other.y; this->z *= other.z; return *this;
	}

	Vector3& operator /=(const T& other) { 
		this->x /= other.x; this->y /= other.y; this->z /= other.z; return *this;
	}

	Vector3 operator +(const Vector3& other) { 
		return Vector3(this->x + other.x, this->y + other.y, this->z + other.z); 
	}

	Vector3 operator -(const Vector3& other) { 
		return Vector3(this->x - other.x, this->y - other.y, this->z - other.z); 
	}

	Vector3 operator *(const T& other) { 
		return Vector3(this->x * other, this->y * other, this->z * other); 
	}

	Vector3 operator /(const T& other) { 
		return Vector3(this->x / other, this->y / other, this->z / other); 
	}

	T getMagnitude() const {
		return sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);
	}			
	
	Vector3 getNormalized() const {
		T r = 1 / getMagnitude(); return  Vector3(this->x * r, this->y * r, this->z * r);
	}

	Vector3& Normalize() { 
		T r = 1 / getMagnitude(); this->x *= r, this->y *= r, this->z *= r; return *this; 
	}

	T getDotProduct(const Vector3& other) const {
		return this->x * other.x + this->y * other.y + this->z * other.z;
	}

	Vector3<T> getCrossProduct(const Vector3& other) { 
		T xCross = this->y * other.z - this->z * other.y;
		T yCross = this->z * other.x - this->x * other.z;
		T zCross = this->x * other.y - this->y * other.x;
		return Vector3<T>(xCross, yCross, zCross);
	}

	void setComponents(std::stringstream& ss)
	{
		char junk;
		ss >> junk >> x >> y >> z;
	}
};

using Vector3f = Vector3<float>; 
using Vector3d = Vector3<double>;
