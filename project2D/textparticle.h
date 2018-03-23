#pragma once

#include <string>

#include "particle.h"

class TextParticle : public Particle
{
public:
	TextParticle(Game* game, Vector2& pos, std::string txt);

	void update(float delta) override;
	void draw(aie::Renderer2D* renderer) override;
private:
	std::string m_text;
};