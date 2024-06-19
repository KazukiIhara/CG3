#pragma once

/// <summary>
/// 2次元ベクトル
/// </summary>
class Vector2
{
public:
	Vector2(float x_ = 0, float y_ = 0)
	{
		x = x_;
		y = y_;
	}
	Vector2 operator+(const Vector2& obj)
	{
		Vector2 v;
		v.x = this->x + obj.x;
		v.y = this->y + obj.y;
		return v;
	}
	Vector2 operator-(const Vector2& obj)
	{
		Vector2 v;
		v.x = this->x - obj.x;
		v.y = this->y - obj.y;
		return v;
	}
	Vector2 operator*(const Vector2& obj)
	{
		Vector2 v;
		v.x = this->x * obj.x;
		v.y = this->y * obj.y;
		return v;
	}
	Vector2 operator/(const Vector2& obj)
	{
		Vector2 v;
		v.x = this->x / obj.x;
		v.y = this->y / obj.y;
		return v;
	}
	void operator+=(const Vector2& obj)
	{
		this->x = this->x + obj.x;
		this->y = this->y + obj.y;
	}
	void operator-=(const Vector2& obj)
	{
		this->x = this->x - obj.x;
		this->y = this->y - obj.y;
	}
	void operator*=(const Vector2& obj)
	{
		this->x = this->x * obj.x;
		this->y = this->y * obj.y;
	}
	void operator/=(const Vector2& obj)
	{
		this->x = this->x / obj.x;
		this->y = this->y / obj.y;
	}

	float x;
	float y;
};