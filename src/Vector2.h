#pragma once

class Vector2
{
public:
	Vector2() = default;
	Vector2(float x, float y);

	Vector2& Set(float x, float y);

	Vector2& Add(const Vector2& rhs);
	Vector2& Sub(const Vector2& rhs);
	Vector2& Scale(float s);

	Vector2& Sub(const Vector2& lhs, const Vector2& rhs);

	float Dot(const Vector2& rhs) const;

	float m_x, m_y;
};