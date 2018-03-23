#pragma once

#include "Renderer2D.h"

#include "tile.h" // for ZONETYPE_COUNT enum
#include "building.h" // for BUILDINGTYPE_COUNT enum

#define UI_PANEL_BUILDINGS 1
#define UI_PANEL_ZONES 2

class Game;

struct Rect
{
	float x, y;
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
	void flashMoney();
private:
	Game* m_game;

	const unsigned int m_panelColour;
	const float m_panelY;

	int m_shownPanel;

	// rect for the panel selector
	Rect m_selectorBox;

	// the positions and destination positions of the panels
	float m_buildingPanelY, m_buildingPanelDestY;
	float m_zonePanelY, m_zonePanelDestY;

	// demand graph position
	Vector2 m_demandGraphPos;
	bool m_mouseOverGraph;
	// local values of zone type demand so we can smooth them
	float m_resDemand, m_comDemand, m_indDemand;

	// used to turn money red to bring attention to it
	float m_moneyFlashTime;

	Rect m_zoneBoxes[ZONETYPE_COUNT];
	Rect m_buildingBoxes[BUILDINGTYPE_COUNT];

	aie::Texture* m_buildingSelectorIcon;
	aie::Texture* m_zoneSelectorIcon;

	// information to show in panels
	unsigned int	m_zoneColours[ZONETYPE_COUNT];
	const char*		m_zoneNames[ZONETYPE_COUNT];
	aie::Texture*	m_buildingIcons[BUILDINGTYPE_COUNT];
	const char*		m_buildingNames[BUILDINGTYPE_COUNT];

	void drawBuildingPanel(aie::Renderer2D* renderer);
	void drawZonePanel(aie::Renderer2D* renderer);

	void drawDemandGraph(aie::Renderer2D* renderer);
	void drawDemandMouseover(aie::Renderer2D* renderer);

	void selectorBoxClicked(int panel);

	bool isMouseInRect(Rect r, float yoffset, bool centerOrigin = false);
};