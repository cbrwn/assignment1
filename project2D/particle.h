#pragma once

#include "Renderer2D.h"

#include "vector2.h"

class Game;

class Particle
{
public:
	Particle(Game* game, Vector2& pos);
	virtual ~Particle() = default;

	virtual void update(float delta) = 0;
	virtual void draw(aie::Renderer2D* renderer);

	inline float getOpacity() { return m_opacity; }
protected:
	Game* m_game;

	Vector2 m_pos, m_vel;
	float m_opacity;
	float m_rotation;
	aie::Texture* m_texture;
};