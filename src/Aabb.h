#pragma once

#include "Vector2.h"

class Aabb
{
public:
    Aabb() = default;
    Aabb(const Vector2& origin, const Vector2& halfExtents);
    Aabb(const SDL_Rect& rect);

    Aabb& Union(const Aabb& other);

    float MinX() const;
    float MaxX() const;
    float MinY() const;
    float MaxY() const;

    void ToRect(SDL_Rect& rect) const;

    Vector2 m_origin;
    Vector2 m_halfExtents;
};
