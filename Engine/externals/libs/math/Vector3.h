#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
class Vector3
{
public:
	Vector3(float x_ = 0, float y_ = 0, float z_ = 0)
	{
		x = x_;
		y = y_;
		z = z_;
	}
	Vector3 operator+(const Vector3& obj)
	{
		Vector3 v;
		v.x = this->x + obj.x;
		v.y = this->y + obj.y;
		v.z = this->z + obj.z;
		return v;
	}
	Vector3 operator-(const Vector3& obj)
	{
		Vector3 v;
		v.x = this->x - obj.x;
		v.y = this->y - obj.y;
		v.z = this->z - obj.z;
		return v;
	}
	Vector3 operator*(const Vector3& obj)
	{
		Vector3 v;
		v.x = this->x * obj.x;
		v.y = this->y * obj.y;
		v.z = this->z * obj.z;
		return v;
	}
	Vector3 operator/(const Vector3& obj)
	{
		Vector3 v;
		v.x = this->x / obj.x;
		v.y = this->y / obj.y;
		v.z = this->z / obj.z;
		return v;
	}
	void operator+=(const Vector3& obj)
	{
		this->x = this->x + obj.x;
		this->y = this->y + obj.y;
		this->z = this->z + obj.z;
	}
	void operator-=(const Vector3& obj)
	{
		this->x = this->x - obj.x;
		this->y = this->y - obj.y;
		this->z = this->z - obj.z;
	}
	void operator*=(const Vector3& obj)
	{
		this->x = this->x * obj.x;
		this->y = this->y * obj.y;
		this->z = this->z * obj.z;
	}
	void operator/=(const Vector3& obj)
	{
		this->x = this->x / obj.x;
		this->y = this->y / obj.y;
		this->z = this->z / obj.z;
	}

	float x;
	float y;
	float z;
};