#include "smokeparticle.h"
#include "game.h"

SmokeParticle::SmokeParticle(Game* game, Vector2& pos)
	: Particle(game, pos)
{
	m_texture = game->getImageManager()->getTexture("smoke");

	// random velocities
	m_vel.setX(((rand() % 10000) - 5000) / 50.0f);
	m_vel.setY(((rand() % 10000) + 2000.0f) / 50.0f);

	// and random rotation
	m_rotateDirection = (rand() % 1000) / 200.0f;
}

void SmokeParticle::update(float delta)
{
	// particle will be removed once opacity reaches 0
	m_opacity -= delta * 1.5f;

	// update transform stuff
	m_rotation += m_rotateDirection * delta;

	m_vel -= (m_vel * delta);
	m_pos += (m_vel * delta);
}