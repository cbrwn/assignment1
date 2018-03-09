#include "smokeparticle.h"
#include "game.h"
#include "random.h"

SmokeParticle::SmokeParticle(Game* game, Vector2& pos)
	: Particle(game, pos)
{
	m_texture = game->getImageManager()->getTexture("smoke");

	// random velocities
	m_vel.setX(randBetween(-100.0f, 100.0f));
	m_vel.setY(randBetween(40.0f, 200.0f));

	// and random rotation
	m_rotateDirection = randBetween(-3.0f, 3.0f);
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