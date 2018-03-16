#pragma once

#include "particle.h"

class PollutionParticle : public Particle
{
public:
	PollutionParticle(Game* game, Vector2& pos);

	void update(float delta) override;
private:
	float m_rotateDirection;
};