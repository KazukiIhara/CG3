#include "MathOperator.h"

Vector2 operator-(const Vector2& v) {
	return Vector2(-v.x, -v.y);
}

Vector2 operator+(const Vector2& v) {
	return v;
}

Vector2 operator+(const Vector2& v1, const Vector2& v2) {
	return Vector2(v1.x + v2.x, v1.y + v2.y);
}

Vector2 operator-(const Vector2& v1, const Vector2& v2) {
	return Vector2(v1.x - v2.x, v1.y - v2.y);
}

Vector2 operator*(const Vector2& v1, const Vector2& v2) {
	return Vector2(v1.x * v2.x, v1.y * v2.y);
}

Vector2 operator/(const Vector2& v1, const Vector2& v2) {
	return Vector2(v1.x / v2.x, v1.y / v2.y);
}

Vector2 operator/(const Vector2& v, float s) {
	return Vector2(v.x / s, v.y / s);
}

Vector2 operator*(float s, const Vector2& v) {
	return Vector2(v.x * s, v.y * s);
}

Vector2 operator*(const Vector2& v, float s) {
	return Vector2(v.x * s, v.y * s);
}

Vector3 operator-(const Vector3& v) {
	return Vector3(-v.x, -v.y, -v.z);
}

Vector3 operator+(const Vector3& v) {
	return v;
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vector3 operator*(const Vector3& v1, const Vector3& v2) {
	return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

Vector3 operator/(const Vector3& v1, const Vector3& v2) {
	return Vector3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

Vector3 operator*(float s, const Vector3& v) {
	return Vector3(v.x * s, v.y * s, v.z * s);
}

Vector3 operator*(const Vector3& v, float s) {
	return Vector3(v.x * s, v.y * s, v.z * s);
}

Vector3 operator/(const Vector3& v, float s) {
	return Vector3(v.x / s, v.y / s, v.z / s);
}

Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) {
	return Matrix4x4();
}

Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) {
	return Matrix4x4();
}

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
	return Matrix4x4();
}
