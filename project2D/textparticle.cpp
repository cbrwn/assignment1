#include "textparticle.h"

#include "game.h"
#include "imagemanager.h"

TextParticle::TextParticle(Game* game, Vector2& pos, const std::string& txt)
	: Particle(game, pos), m_text(txt)
{
	m_texture = nullptr;

	// random velocities
	m_vel.setX(0);
	m_vel.setY(100.0f);
}

void TextParticle::update(float delta)
{
	// particle will be removed once opacity reaches 0
	m_opacity -= delta * 0.8f;

	m_pos += (m_vel * delta);
}


void TextParticle::draw(aie::Renderer2D* renderer)
{
	renderer->setRenderColour(0.5f, 0.0f, 0.0f, m_opacity);
	renderer->drawText(m_game->m_uiFontLarge, m_text.c_str(),
		m_pos.getX(), m_pos.getY());
}