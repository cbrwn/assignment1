#pragma once

#include <Renderer2D.h>

#define UI_SELECTION_BOX_COUNT 4

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

	aie::Texture** m_tileImages;

	float m_panelHeight;
	const unsigned int m_panelColour;

	float	m_boxPositions[UI_SELECTION_BOX_COUNT];
	float	m_boxPositionY;
	float	m_boxWidth;
	int		m_mouseOverIndex;

	bool isInsideBox(int x, int y, int boxIndex);
};