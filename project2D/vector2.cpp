#include <cmath>
#include "vector2.h"

Vector2::Vector2()
{
	Vector2::Vector2(0, 0);
}

Vector2::Vector2(float x, float y)
	: m_x(x), m_y(y)
{ }

float Vector2::magnitude()
{
	return sqrtf((m_x*m_x) + (m_y*m_y));
}

// magnitude without the need for sqrt
// useful for comparisons when the actual magnitude isn't required
float Vector2::magnitudeSquared()
{
	return (m_x*m_x) + (m_y*m_y);
}

Vector2 Vector2::normalized()
{
	float mag = this->magnitude();
	return Vector2(m_x / mag, m_y / mag);
}

float Vector2::angle()
{
	return atan2f(m_y, m_x);
}

float Vector2::distanceTo(Vector2& vec)
{
	float distX = m_x - vec.getX();
	float distY = m_y - vec.getY();
	return sqrtf((distX*distX) + (distY*distY));
}

// distance without the need for sqrt
// useful for comparisons, when the actual distance isn't required
float Vector2::distanceToSquared(Vector2& vec)
{
	float distX = m_x - vec.getX();
	float distY = m_y - vec.getY();
	return (distX*distX) + (distY*distY);
}

Vector2 Vector2::operator+(Vector2& vec)
{
	return Vector2(m_x + vec.getX(), m_y + vec.getY());
}

Vector2 Vector2::operator-(Vector2& vec)
{
	return Vector2(m_x - vec.getX(), m_y - vec.getY());
}

Vector2 Vector2::operator*(float mul)
{
	return Vector2(m_x * mul, m_y * mul);
}

Vector2& Vector2::operator=(Vector2& vec)
{
	m_x = vec.getX();
	m_y = vec.getY();
	return *this;
}

Vector2& Vector2::operator+=(Vector2& vec)
{
	m_x += vec.getX();
	m_y += vec.getY();
	return *this;
}

Vector2& Vector2::operator-=(Vector2& vec)
{
	m_x -= vec.getX();
	m_y -= vec.getY();
	return *this;
}

// returns the dot product of two vectors
float Vector2::operator*(Vector2& vec)
{
	return m_x * vec.getX() + m_y * vec.getY();
}

// might be kinda useless with float inaccuracies
bool Vector2::operator==(Vector2& vec)
{
	return (m_x == vec.getX() && m_y == vec.getY());
}
