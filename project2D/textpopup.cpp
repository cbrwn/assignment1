#include <iostream>

#include "textpopup.h"

TextPopup::TextPopup(char* text, float startX, float startY, float lifeTime, float ySpeed, aie::Font* font)
	: m_text(text), m_posX(startX), m_posY(startY), m_life(lifeTime), m_speedY(ySpeed), m_font(font)
{
}

TextPopup::~TextPopup()
{
	delete m_text;
}

void TextPopup::update(float delta)
{
	m_life -= delta;
	if (m_life <= 0)
		return;

	m_posY += m_speedY * delta;
}

void TextPopup::draw(aie::Renderer2D* renderer)
{
	if (m_life <= 0)
		return;
	renderer->setRenderColour(0, 0.5f, 0, m_life);
	renderer->drawText(m_font, m_text, m_posX, m_posY);
}
