#include "stdafx.h"

#include "Aabb.h"

Aabb::Aabb(const Vector2& origin, const Vector2& halfExtents)
	: m_origin(origin)
	, m_halfExtents(halfExtents)
{

}

Aabb::Aabb(const SDL_Rect& rect)
{
	m_halfExtents.m_x = rect.w * 0.5f;
	m_halfExtents.m_y = rect.h * 0.5f;
	m_origin.m_x = rect.x + m_halfExtents.m_x;
	m_origin.m_y = rect.y + m_halfExtents.m_y;
}

Aabb& Aabb::Union(const Aabb& other)
{
	float minX = min(MinX(), other.MinX());
	float minY = min(MinY(), other.MinY());
	float maxX = max(MaxX(), other.MaxX());
	float maxY = max(MaxY(), other.MaxY());

	m_origin.m_x = (minX + maxX) * 0.5f;
	m_origin.m_y = (minY + maxY) * 0.5f;

	m_halfExtents.m_x = (maxX - minX) * 0.5f;
	m_halfExtents.m_y = (maxY - minY) * 0.5f;

	return *this;
}

float Aabb::MinX() const
{
	return m_origin.m_x - m_halfExtents.m_x;
}

float Aabb::MaxX() const
{
	return m_origin.m_x + m_halfExtents.m_x;
}

float Aabb::MinY() const
{
	return m_origin.m_y - m_halfExtents.m_y;
}

float Aabb::MaxY() const
{
	return m_origin.m_y + m_halfExtents.m_y;
}

void Aabb::ToRect(SDL_Rect& rect) const
{
	rect.x = static_cast<int>(m_origin.m_x - m_halfExtents.m_x);
	rect.y = static_cast<int>(m_origin.m_y - m_halfExtents.m_y);
	rect.w = static_cast<int>(m_halfExtents.m_x * 2.0f);
	rect.h = static_cast<int>(m_halfExtents.m_y * 2.0f);
}
