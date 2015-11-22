#include "stdafx.h"

#include "Aabb.h"
#include "Collision.h"
#include "Vector2.h"

namespace Collision
{

bool AabbVsAabbInternal(const Vector2& delta, const Vector2& origin, const Vector2& halfExtents, const Vector2& point,
                        Vector2& normal, float& distance)
{
	if (abs(delta.m_x) > abs(delta.m_y))
	{
		normal.Set(delta.m_x > 0.0f ? -1.0f : 1.0f, 0.0f);
	}
	else
	{
		normal.Set(0.0f, delta.m_y > 0.0f ? -1.0f : 1.0f);
	}

	Vector2 planeCentre(normal);
	planeCentre.Mul(halfExtents);
	planeCentre.Add(origin);

	Vector2 planeDelta(point);
	planeDelta.Sub(planeCentre);
	distance = planeDelta.Dot(normal);

	return true;
}

bool AabbVsAabb(const Aabb& a, const Aabb& b, Vector2& normal, float& distance)
{
	Vector2 combinedExtents(b.m_halfExtents);
	combinedExtents.Add(a.m_halfExtents);
	Vector2 combinedPos(b.m_origin);
	Vector2 delta(combinedPos);
	delta.Sub(a.m_origin);
	AabbVsAabbInternal(delta, combinedPos, combinedExtents, a.m_origin, normal, distance);
	return true;
}

}