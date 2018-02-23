#pragma once

#include <Renderer2D.h>

class Game;

class UiManager
{
public:
	UiManager(Game* game);
	~UiManager();

	void update(float delta);
	void draw(aie::Renderer2D* renderer);

	bool isMouseOverUi();
private:
	Game* m_game;

	float m_panelHeight;
	const unsigned int m_panelColour;
};