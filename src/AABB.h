#pragma once

#include "Vector2.h"

class AABB
{
public:
	AABB() = default;
	AABB(const Vector2& origin, const Vector2& halfExtents);
	AABB(const SDL_Rect& rect);

	AABB& Union(const AABB& other);

	float MinX() const;
	float MaxX() const;
	float MinY() const;
	float MaxY() const;

	void ToRect(SDL_Rect& rect) const;

	Vector2 m_origin;
	Vector2 m_halfExtents;
};