#pragma once

#include "particle.h"

class SmokeParticle : public Particle
{
public:
	SmokeParticle(Game* game, float x, float y);

	void update(float delta) override;
private:
	float m_rotateDirection;
};