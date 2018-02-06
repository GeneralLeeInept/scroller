#pragma once

#include "Vector2.h"

class Aabb;

namespace Collision
{

struct Hit
{
    Vector2 m_normal;
    float m_distance;
};

typedef function<bool(const Hit& hit)> HitFilter;

bool AabbVsAabb(const Aabb& a, const Aabb& b, const HitFilter& hitFilter, Hit& hit);
} // namespace Collision
