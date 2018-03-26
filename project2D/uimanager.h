#pragma once

#include "Renderer2D.h"

#include "tile.h" // for ZONETYPE_COUNT enum
#include "building.h" // for BUILDINGTYPE_COUNT enum

// these could be an enum
#define UI_PANEL_BUILDINGS 1
#define UI_PANEL_ZONES 2

// Forward declares
class Game;

// struct to represent boxes for both drawing and clicking
struct Rect
{
	float x, y;
	float width, height;
};

class UiManager
{
public:
	//------------------------------------------------------------------------
	// (explicit because we don't want any implicit conversion)
	//
	// Param:
	//			game: pointer to our Game so we can access everything we need
	//------------------------------------------------------------------------
	explicit UiManager(Game* game);
	~UiManager();

	//------------------------------------------------------------------------
	// Called every frame to deal with everything non-drawing related
	//
	// Param: 
	//			delta: time in seconds since the last frame
	//------------------------------------------------------------------------
	void update(float delta);
	//------------------------------------------------------------------------
	// Also called every frame, dealing with everything drawing related
	//
	// Param: 
	//			renderer: a pointer to the Renderer2D we're using
	//------------------------------------------------------------------------
	void draw(aie::Renderer2D* renderer);

	//------------------------------------------------------------------------
	// Sets the current panel to show
	//
	// Param: 
	//			panel: ID of panel to be shown
	//------------------------------------------------------------------------
	void setShownPanel(int panel);

	//------------------------------------------------------------------------
	// Checks if the mouse is over the UI
	// Used for checking if the player is interacting with the game or not
	//
	// Return: 
	//			whether the mouse is over any part of the UI
	//------------------------------------------------------------------------
	bool isMouseOverUi();
	//------------------------------------------------------------------------
	// Begins flashing the money text
	// Used when trying to buy a building without the sufficient money
	//------------------------------------------------------------------------
	void flashMoney();
private:
	Game* m_game;

	// background colour for panels
	const unsigned int m_panelColour;
	// the destination Y position for panels when they're open
	const float m_panelY;

	// which panel is supposed to be shown
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

	// boxes for selecting zones and buildings
	Rect m_zoneBoxes[ZONETYPE_COUNT];
	Rect m_buildingBoxes[BUILDINGTYPE_COUNT];

	// icons for the selector box
	aie::Texture* m_buildingSelectorIcon;
	aie::Texture* m_zoneSelectorIcon;

	// information to show in panels
	unsigned int	m_zoneColours[ZONETYPE_COUNT];
	const char*		m_zoneNames[ZONETYPE_COUNT];
	aie::Texture*	m_buildingIcons[BUILDINGTYPE_COUNT];
	const char*		m_buildingNames[BUILDINGTYPE_COUNT];

	// drawing each panel
	void drawBuildingPanel(aie::Renderer2D* renderer);
	void drawZonePanel(aie::Renderer2D* renderer);

	// drawing demand-related elements
	void drawDemandGraph(aie::Renderer2D* renderer);
	void drawDemandMouseover(aie::Renderer2D* renderer);

	// called when the selector box is clicked to determine which panel
	// to show
	void selectorBoxClicked(int panel);

	// for checking if the player clicked on things
	bool isMouseInRect(Rect r, float yoffset, bool centerOrigin = false);
};