#include "stdafx.h"

#include "Aabb.h"
#include "Collision.h"
#include "Vector2.h"

namespace Collision
{

bool AabbVsAabbInternal(const Vector2& delta, const Vector2& origin, const Vector2& halfExtents, const Vector2& point, Hit& hit)
{
	if (abs(delta.m_x) < abs(delta.m_y))
	{
		hit.m_normal.Set(0.0f, delta.m_y > 0.0f ? -1.0f : 1.0f);
	}
	else
	{
		hit.m_normal.Set(delta.m_x > 0.0f ? -1.0f : 1.0f, 0.0f);
	}

	Vector2 planeCentre;
	planeCentre.Mul(hit.m_normal, halfExtents);
	planeCentre.Add(origin);

	Vector2 planeDelta(point);
	planeDelta.Sub(planeCentre);
	hit.m_distance = planeDelta.Dot(hit.m_normal);

	return true;
}

bool AabbVsAabb(const Aabb& a, const Aabb& b, const HitFilter& hitFilter, Hit& hit)
{
	Vector2 combinedExtents;
	combinedExtents.Add(a.m_halfExtents, b.m_halfExtents);
	Vector2 combinedPos(b.m_origin);
	Vector2 delta;
	delta.Sub(combinedPos, a.m_origin);
	
	AabbVsAabbInternal(delta, combinedPos, combinedExtents, a.m_origin, hit);
	return !hitFilter(hit);
}

}