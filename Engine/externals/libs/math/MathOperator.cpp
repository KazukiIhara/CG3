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
	return Matrix4x4{
		m1.m[0][0] + m2.m[0][0], m1.m[0][1] + m2.m[0][1], m1.m[0][2] + m2.m[0][2], m1.m[0][3] + m2.m[0][3],
		m1.m[1][0] + m2.m[1][0], m1.m[1][1] + m2.m[1][1], m1.m[1][2] + m2.m[1][2], m1.m[1][3] + m2.m[1][3],
		m1.m[2][0] + m2.m[2][0], m1.m[2][1] + m2.m[2][1], m1.m[2][2] + m2.m[2][2], m1.m[2][3] + m2.m[2][3],
		m1.m[3][0] + m2.m[3][0], m1.m[3][1] + m2.m[3][1], m1.m[3][2] + m2.m[3][2], m1.m[3][3] + m2.m[3][3]
	};
}

Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) {
	return Matrix4x4{
		m1.m[0][0] - m2.m[0][0], m1.m[0][1] - m2.m[0][1], m1.m[0][2] - m2.m[0][2], m1.m[0][3] - m2.m[0][3],
		m1.m[1][0] - m2.m[1][0], m1.m[1][1] - m2.m[1][1], m1.m[1][2] - m2.m[1][2], m1.m[1][3] - m2.m[1][3],
		m1.m[2][0] - m2.m[2][0], m1.m[2][1] - m2.m[2][1], m1.m[2][2] - m2.m[2][2], m1.m[2][3] - m2.m[2][3],
		m1.m[3][0] - m2.m[3][0], m1.m[3][1] - m2.m[3][1], m1.m[3][2] - m2.m[3][2], m1.m[3][3] - m2.m[3][3]
	};
}

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
	return Matrix4x4{
		m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0],		m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1],		m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2],		m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3],
		m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0],		m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1],		m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2],		m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3],
		m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0],		m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1],		m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2],		m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3],
		m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0],		m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1],		m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2],		m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3],
	};
}
