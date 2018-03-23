#include "pollutionparticle.h"

#include "game.h"
#include "random.h"
#include "imagemanager.h"

PollutionParticle::PollutionParticle(Game* game, Vector2& pos)
	: Particle(game, pos)
{
	m_texture = game->getImageManager()->getTexture("pollution");

	// random velocities
	m_vel.setX(randBetween(-10.0f, 10.0f));
	m_vel.setY(randBetween(40.0f, 100.0f));

	// and random rotation
	m_rotateDirection = randBetween(-3.0f, 3.0f);
}

void PollutionParticle::update(float delta)
{
	// particle will be removed once opacity reaches 0
	m_opacity -= delta * 0.8f;

	// update transform stuff
	m_rotation += m_rotateDirection * delta;

	m_pos += (m_vel * delta);
}