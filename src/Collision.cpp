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

	Vector2 planeCentre;
	planeCentre.Mul(normal, halfExtents);
	planeCentre.Add(origin);

	Vector2 planeDelta(point);
	planeDelta.Sub(planeCentre);
	distance = planeDelta.Dot(normal);

	return true;
}

bool AabbVsAabb(const Aabb& a, const Aabb& b, Vector2& normal, float& distance)
{
	Vector2 combinedExtents;
	combinedExtents.Add(a.m_halfExtents, b.m_halfExtents);
	Vector2 combinedPos(b.m_origin);
	Vector2 delta;
	delta.Sub(combinedPos, a.m_origin);
	AabbVsAabbInternal(delta, combinedPos, combinedExtents, a.m_origin, normal, distance);

	return true;
}

bool IntersectSegment(const Aabb& a, const Vector2& segmentStart, const Vector2& delta, float paddingX, float paddingY,
                      Vector2& normal, float& t)
{
	float scaleX = 1.0f / delta.m_x;
	float scaleY = 1.0f / delta.m_y;
	float signX = scaleX < 0.0f ? -1.0f : 1.0f;
	float signY = scaleY < 0.0f ? -1.0f : 1.0f;
	float nearTimeX = (a.m_origin.m_x - signX * (a.m_halfExtents.m_x + paddingX) - segmentStart.m_x) * scaleX;
	float nearTimeY = (a.m_origin.m_y - signY * (a.m_halfExtents.m_y + paddingY) - segmentStart.m_y) * scaleY;
	float farTimeX = (a.m_origin.m_x + signX * (a.m_halfExtents.m_x + paddingX) - segmentStart.m_x) * scaleX;
	float farTimeY = (a.m_origin.m_y + signY * (a.m_halfExtents.m_y + paddingY) - segmentStart.m_y) * scaleY;

	if (nearTimeX > farTimeY || nearTimeY > farTimeX)
	{
		return false;
	}

	float nearTime = (nearTimeX > nearTimeY) ? nearTimeX : nearTimeY;
	float farTime = (farTimeX < farTimeY) ? farTimeX : farTimeY;

	if (nearTime > 1.0f || farTime < 0.0f)
	{
		return false;
	}

	t = max(0.0f, min(nearTime, 1.0f));

	if (nearTimeX > nearTimeY)
	{
		normal.Set(-signX, 0.0f);
	}
	else
	{
		normal.Set(0.0f, -signY);
	}

	return true;
}

// Test if moving Aabb a hits stationary Aabb b
bool SweepAabb(const Aabb& a, const Aabb& b, const Vector2& delta, Vector2& normal, float& distance)
{
	return IntersectSegment(b, a.m_origin, delta, a.m_halfExtents.m_x, a.m_halfExtents.m_y, normal, distance);
}

}