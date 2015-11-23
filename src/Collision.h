#pragma once

class Aabb;
class Vector2;

namespace Collision
{
bool AabbVsAabb(const Aabb& a, const Aabb& b, Vector2& normal, float& distance);
bool SweepAabb(const Aabb& a, const Aabb& b, const Vector2& delta, Vector2& normal, float& distance);
}