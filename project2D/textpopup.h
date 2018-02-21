#pragma once

#include <Renderer2D.h>

class TextPopup
{
public:
	TextPopup(char* text, float startX, float startY, float lifeTime, float ySpeed, aie::Font* font);
	~TextPopup();

	void update(float delta);
	void draw(aie::Renderer2D* renderer);

	bool shouldDie() { return m_life <= 0.0f; }
private:
	char* m_text;
	aie::Font* m_font;
	float m_posX, m_posY;
	float m_speedY;
	float m_life;
};