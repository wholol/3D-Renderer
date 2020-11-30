#pragma once
#include <sstream>
//operator overlaoded vector class
template <typename T>
class Vector2 {
public:
	T x, y;			//equivalent to i,j,k.

	Vector2() : x(0), y(0) {}
	Vector2(T x, T y, T z) : x(x), y(y) {}

	Vector2& operator +=(const Vector2& other)
	{
		this->x += other.x; this->y += other.y; return *this;
	}

	Vector2& operator -=(const Vector2& other) {
		this->x -= other.x; this->y -= other.y; return *this;
	}

	Vector2& operator *=(const T& other) {
		this->x *= other.x; this->y *= other.y; return *this;
	}

	Vector2& operator /=(const T& other) {
		this->x /= other.x; this->y /= other.y; return *this;
	}

	Vector2 operator +(const Vector2& other) {
		return Vector2(this->x + other.x, this->y + other.y);
	}

	Vector2 operator-(const Vector2& other) {
		return Vector2(this->x - other.x, this->y - other.y);
	}

	Vector2 operator *(const T& other) {
		return Vector2(this->x * other, this->y * other);
	}

	Vector2 operator /(const T& other) {
		return Vector2(this->x / other.x, this->y / other.y);
	}

	T getMagnitude() const {
		return sqrt(this->x * this->x + this->y * this->y);
	}

	Vector2 getNormalized() const {
		T r = 1 / getMagnitude(); return  Vector2(this->x * r, this->y * r);
	}

	Vector2& Normalize() {
		T r = 1 / getMagnitude(); this->x *= r, this->y *= r; return *this;
	}

	T getDotProduct(const Vector2& other) const {
		return this->x * other.x + this->y * other.y;
	}

	Vector2<T> getCrossProduct(const Vector2& other) {
		T xCross = this->y * other.x - this->x * other.y;
		T yCross = this->y * other.x - this->x * other.y;
		return Vector2<T>(xCross, yCross);
	}

	void setComponents(std::stringstream& ss)
	{
		char junk;
		ss >> junk >> x >> y;
	}
};

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;
