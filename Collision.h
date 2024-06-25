#pragma once
#include "MathOperator.h"

class cCollision {
public:
	struct AABB {
		Vector3 min;
		Vector3 max;
	};

public:
	static bool IsCollision(const AABB& aabb, const Vector3& position);

};

