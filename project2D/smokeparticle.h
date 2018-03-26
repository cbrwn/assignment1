#pragma once

#include "particle.h"

class SmokeParticle : public Particle
{
public:
	SmokeParticle(Game* game, Vector2& pos);

	void update(float delta) override;
private:
	// the speed at which this rotates
	float m_rotateVelocity;
};