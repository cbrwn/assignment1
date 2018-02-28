#include "particle.h"

Particle::Particle(Game* game, float x, float y)
	: m_game(game), m_posX(x), m_posY(y)
{
	m_opacity = 1.0f;
	m_velX = 0.0f;
	m_velY = 0.0f;
	m_rotation = 0.0f;
	m_texture = nullptr;
}

void Particle::draw(aie::Renderer2D* renderer)
{
	renderer->setRenderColour(1, 1, 1, m_opacity);
	renderer->drawSprite(m_texture, m_posX, m_posY, 0, 0, m_rotation);
}
