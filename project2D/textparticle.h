#pragma once

#include <string>

#include "particle.h"

class TextParticle : public Particle
{
public:
	TextParticle(Game* game, Vector2& pos, const std::string& txt);

	void update(float delta) override;
	void draw(aie::Renderer2D* renderer) override;
private:
	// text to show
	std::string m_text;
};