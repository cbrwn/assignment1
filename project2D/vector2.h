#pragma once

class Vector2
{
public:
	Vector2();
	Vector2(float x, float y);
	~Vector2() = default;

	Vector2(const Vector2& vec);

	// move constructors aren't needed
	Vector2(Vector2&& vec) = default;
	Vector2& operator=(Vector2&& vec) = default;

	void setX(float x) { m_x = x; }
	void setY(float y) { m_y = y; }
	inline void setPos(float x, float y);

	float getX() const { return m_x; }
	float getY() const { return m_y; }

	void addX(float amt) { m_x += amt; }
	void addY(float amt) { m_y += amt; }
	void subX(float amt) { m_x -= amt; }
	void subY(float amt) { m_y -= amt; }

	float magnitude();
	float magnitudeSquared();
	Vector2 normalized();

	float angle();

	float distanceTo(Vector2& vec);
	float distanceToSquared(Vector2& vec);

	// operator overloads
	Vector2  operator+ (Vector2& vec);
	Vector2  operator- (Vector2& vec);
	Vector2  operator* (float mul);
	Vector2& operator= (Vector2& vec);
	Vector2& operator+=(Vector2& vec);
	Vector2& operator-=(Vector2& vec);
	float    operator* (Vector2& vec);
	bool	 operator==(Vector2& vec);
private:
	float m_x, m_y;
};

// multi-line inlines
inline void Vector2::setPos(float x, float y)
{
	m_x = x;
	m_y = y;
}