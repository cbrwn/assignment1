#pragma once

#include <Renderer2D.h>

class Game;

class Particle
{
public:
	Particle(Game* game, float x, float y);

	virtual void update(float delta) = 0;
	void draw(aie::Renderer2D* renderer);

	inline float getOpacity() { return m_opacity; }
protected:
	Game* m_game;

	float m_posX, m_posY;
	float m_velX, m_velY;
	float m_opacity;
	float m_rotation;
	aie::Texture* m_texture;
};