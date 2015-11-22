#include "stdafx.h"

#include "Aabb.h"
#include "Collision.h"
#include "Vector2.h"

namespace Collision
{

bool AabbVsAabb(const Aabb& a, const Aabb& b, Vector2& normal, float& distance)
{
	Vector2 d;
	d.Sub(a.m_origin, b.m_origin);

	Vector2 extents(a.m_halfExtents);
	extents.Add(b.m_halfExtents);

	float edgeDist;

	if (abs(d.m_x) > abs(d.m_y))
	{
		normal.Set(d.m_x > 0.0f ? 1.0f : -1.0f, 0.0f);
		edgeDist = extents.m_x;
	}
	else
	{
		normal.Set(0.0f, d.m_y > 0.0f ? 1.0f : -1.0f);
		edgeDist = extents.m_y;
	}

	distance = normal.Dot(a.m_origin) - normal.Dot(b.m_origin) - edgeDist;

	return true;
}

}