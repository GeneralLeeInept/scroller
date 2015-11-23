#include "stdafx.h"

#include "Vector2.h"

Vector2::Vector2()
	: Vector2(0.0f, 0.0f)
{

}

Vector2::Vector2(float x, float y)
	: m_x(x)
	, m_y(y)
{

}

Vector2& Vector2::Set(float x, float y)
{
	m_x = x;
	m_y = y;
	return *this;
}

Vector2& Vector2::Add(const Vector2& rhs)
{
	m_x += rhs.m_x;
	m_y += rhs.m_y;
	return *this;
}

Vector2& Vector2::Sub(const Vector2& rhs)
{
	m_x -= rhs.m_x;
	m_y -= rhs.m_y;
	return *this;
}

Vector2& Vector2::Mul(const Vector2& rhs)
{
	m_x *= rhs.m_x;
	m_y *= rhs.m_y;
	return *this;
}

Vector2& Vector2::Scale(float s)
{
	m_x *= s;
	m_y *= s;
	return *this;
}

Vector2& Vector2::Add(const Vector2& lhs, const Vector2& rhs)
{
	m_x = lhs.m_x + rhs.m_x;
	m_y = lhs.m_y + rhs.m_y;
	return *this;
}

Vector2& Vector2::Sub(const Vector2& lhs, const Vector2& rhs)
{
	m_x = lhs.m_x - rhs.m_x;
	m_y = lhs.m_y - rhs.m_y;
	return *this;
}

Vector2& Vector2::Mul(const Vector2& lhs, const Vector2& rhs)
{
	m_x = lhs.m_x * rhs.m_x;
	m_y = lhs.m_y * rhs.m_y;
	return *this;
}

Vector2& Vector2::Scale(const Vector2& v, float s)
{
	m_x = v.m_x * s;
	m_y = v.m_y * s;
	return *this;
}

float Vector2::Dot(const Vector2& rhs) const
{
	return (m_x * rhs.m_x) + (m_y * rhs.m_y);
}
