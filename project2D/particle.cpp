#include "particle.h"

Particle::Particle(Game* game, Vector2& pos)
	: m_game(game), m_pos(pos)
{
	m_opacity = 1.0f;
	m_rotation = 0.0f;
	m_texture = nullptr;
}

void Particle::draw(aie::Renderer2D* renderer)
{
	renderer->setRenderColour(1, 1, 1, m_opacity);
	renderer->drawSprite(m_texture, m_pos.getX(), m_pos.getY(), 0, 0, m_rotation);
}
