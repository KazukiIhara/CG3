#include "Collision.h"

bool cCollision::IsCollision(const AABB& aabb, const Vector3& position) {
	return (position.x >= aabb.min.x && position.x <= aabb.max.x &&
		position.y >= aabb.min.y && position.y <= aabb.max.y &&
		position.z >= aabb.min.z && position.z <= aabb.max.z);
}
