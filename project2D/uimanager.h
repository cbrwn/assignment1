#pragma once

#include <Renderer2D.h>

#include "building.h"
#include "tile.h"

#define UI_PANEL_BUILDINGS 1
#define UI_PANEL_ZONES 2

class Game;

struct Rect
{
	float left, top;
	float width, height;
};

class UiManager
{
public:
	UiManager(Game* game);
	~UiManager();

	void update(float delta);
	void draw(aie::Renderer2D* renderer);

	void setShownPanel(int panel);

	bool isMouseOverUi();
private:
	Game* m_game;

	const unsigned int m_panelColour;
	const float m_panelY;

	int m_shownPanel;

	// the positions and destination positions of the panels
	float m_buildingPanelY, m_buildingPanelDestY;
	float m_zonePanelY, m_zonePanelDestY;

	Rect m_zoneBoxes[ZONETYPE_COUNT];
	Rect m_buildingBoxes[BUILDINGTYPE_COUNT];

	void drawBuildingPanel(aie::Renderer2D* renderer);
	void drawZonePanel(aie::Renderer2D* renderer);

	bool isMouseInRect(Rect r, float yoffset);
};