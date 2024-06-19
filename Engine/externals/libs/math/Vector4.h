#pragma once

/// <summary>
/// 4次元ベクトル
/// </summary>
class Vector4
{
public:
	Vector4(float x_ = 0, float y_ = 0, float z_ = 0, float w_ = 0)
	{
		x = x_;
		y = y_;
		z = z_;
		w = w_;
	}
	Vector4 operator+(const Vector4& obj)
	{
		Vector4 v;
		v.x = this->x + obj.x;
		v.y = this->y + obj.y;
		v.z = this->z + obj.z;
		v.w = this->w + obj.w;
		return v;
	}
	Vector4 operator-(const Vector4& obj)
	{
		Vector4 v;
		v.x = this->x - obj.x;
		v.y = this->y - obj.y;
		v.z = this->z - obj.z;
		v.w = this->w - obj.w;
		return v;
	}
	Vector4 operator*(const Vector4& obj)
	{
		Vector4 v;
		v.x = this->x * obj.x;
		v.y = this->y * obj.y;
		v.z = this->z * obj.z;
		v.w = this->w * obj.w;
		return v;
	}
	Vector4 operator/(const Vector4& obj)
	{
		Vector4 v;
		v.x = this->x / obj.x;
		v.y = this->y / obj.y;
		v.z = this->z / obj.z;
		v.w = this->w / obj.w;
		return v;
	}
	void operator+=(const Vector4& obj)
	{
		this->x = this->x + obj.x;
		this->y = this->y + obj.y;
		this->z = this->z + obj.z;
		this->w = this->w + obj.w;
	}
	void operator-=(const Vector4& obj)
	{
		this->x = this->x - obj.x;
		this->y = this->y - obj.y;
		this->z = this->z - obj.z;
		this->w = this->w - obj.w;
	}
	void operator*=(const Vector4& obj)
	{
		this->x = this->x * obj.x;
		this->y = this->y * obj.y;
		this->z = this->z * obj.z;
		this->w = this->w * obj.w;
	}
	void operator/=(const Vector4& obj)
	{
		this->x = this->x / obj.x;
		this->y = this->y / obj.y;
		this->z = this->z / obj.z;
		this->w = this->w / obj.w;
	}

	float x;
	float y;
	float z;
	float w;
};