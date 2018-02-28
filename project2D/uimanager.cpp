////////////////////////////////////////////
// UiManager
// Base class for all buildings which can be
//    placed in the world
////////////////////////////////////////////

#include <Input.h>
#include <Font.h>

#include "game.h"
#include "uimanager.h"
#include "buildingmanager.h"
#include "zonemanager.h"

#include "tile.h"

UiManager::UiManager(Game* game)
	: m_game(game), m_panelColour(0x444444ff), m_panelY(120.0f)
{
	m_shownPanel = 0;

	m_buildingPanelDestY = -50;
	m_buildingPanelY = m_buildingPanelDestY;
	m_zonePanelDestY = -50;
	m_zonePanelY = m_zonePanelDestY;

	// make all the rects for selection boxes
	const float selectBoxWidth = 86.0f;
	const float selectBoxPadding = 16.0f;
	// zone boxes
	float currentX = selectBoxPadding;
	for (int i = 0; i < ZONETYPE_COUNT; i++)
	{
		m_zoneBoxes[i] = { currentX, m_panelY - selectBoxPadding,
			selectBoxWidth, m_panelY - (selectBoxPadding * 2.0f) };
		currentX += selectBoxWidth + selectBoxPadding;
	}

	// building boxes
	currentX = selectBoxPadding;
	for (int i = 0; i < BUILDINGTYPE_COUNT; i++)
	{
		m_buildingBoxes[i] = { currentX, m_panelY - selectBoxPadding,
			selectBoxWidth, m_panelY - (selectBoxPadding * 2.0f) };
		currentX += selectBoxWidth + selectBoxPadding;
	}

	// selector box

	m_selectorBox = { 32, game->getWindowHeight() - 16.0f, 64, 32 };

	// set the information we'll be showing in the panels
	m_zoneColours[ZONETYPE_NONE] = 0x00000000; // don't draw a box
	m_zoneColours[ZONETYPE_RESIDENTIAL] = 0x00ff00ff; // green
	m_zoneColours[ZONETYPE_COMMERCIAL] = 0x0000ffff; // blue
	m_zoneColours[ZONETYPE_INDUSTRIAL] = 0xffff00ff; // yellow

	m_zoneNames[ZONETYPE_NONE] = "De-zone";
	m_zoneNames[ZONETYPE_RESIDENTIAL] = "Residential";
	m_zoneNames[ZONETYPE_COMMERCIAL] = "Commercial";
	m_zoneNames[ZONETYPE_INDUSTRIAL] = "Industrial";

	ImageManager* img = m_game->getImageManager();
	m_buildingIcons[BUILDINGTYPE_NONE] = img->getTexture("icons/demolish");
	m_buildingIcons[BUILDINGTYPE_POWERPLANT] = img->getTexture("icons/powerplant");
	m_buildingIcons[BUILDINGTYPE_POWERPOLE] = img->getTexture("icons/powerpole");
	
	m_buildingNames[BUILDINGTYPE_NONE] = "Demolish";
	m_buildingNames[BUILDINGTYPE_POWERPLANT] = "Power Plant";
	m_buildingNames[BUILDINGTYPE_POWERPOLE] = "Power Pole";
}

UiManager::~UiManager()
{
}

void UiManager::update(float delta)
{
	const float smoothSpeed = 10.0f;
	m_buildingPanelY -= (m_buildingPanelY - m_buildingPanelDestY) * delta * smoothSpeed;
	m_zonePanelY -= (m_zonePanelY - m_zonePanelDestY) * delta * smoothSpeed;

	// do input
	aie::Input* input = aie::Input::getInstance();
	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT)
		&& isMouseOverUi())
	{
		// check for zone panel clickage when it's visible
		if (m_zonePanelY >= 0.0f)
			for (int i = 0; i < ZONETYPE_COUNT; i++)
				if (isMouseInRect(m_zoneBoxes[i], m_zonePanelY))
				{
					m_game->getZoneManager()->setSelectedType((ZoneType)i);
					break;
				}

		// and also building panel clickage
		if (m_buildingPanelY >= 0.0f)
			for (int i = 0; i < BUILDINGTYPE_COUNT; i++)
				if (isMouseInRect(m_buildingBoxes[i], m_buildingPanelY))
				{
					m_game->getBuildingManager()->setSelectedBuilding(i);
					break;
				}
	}

	// make sure the selector box is always at the top of the screen
	m_selectorBox.y = m_game->getWindowHeight() - m_selectorBox.height/2.0f;
}

void UiManager::draw(aie::Renderer2D* renderer)
{
	// only want to draw the panels if they're on screen
	if (m_buildingPanelY >= 0.0f)
		drawBuildingPanel(renderer);
	if (m_zonePanelY >= 0.0f)
		drawZonePanel(renderer);

	// draw selector box
	renderer->setRenderColour(m_panelColour);
	renderer->drawBox(m_selectorBox.x, m_selectorBox.y, m_selectorBox.width,
		m_selectorBox.height);
	renderer->setRenderColour(m_panelColour - 0x33333333);
	renderer->drawLine(m_selectorBox.x, m_selectorBox.y - m_selectorBox.height/3.0f, 
		m_selectorBox.x, m_selectorBox.y + m_selectorBox.height/3.0f);
}

void UiManager::setShownPanel(int panel)
{
	// set the destination to below 0 to hide the whole panel 
	//    (including the title)
	const float hiddenValue = -50.0f;
	m_shownPanel = panel;
	m_buildingPanelDestY = panel == UI_PANEL_BUILDINGS ? m_panelY : hiddenValue;
	m_zonePanelDestY = panel == UI_PANEL_ZONES ? m_panelY : hiddenValue;
}

bool UiManager::isMouseOverUi()
{
	int mx, my;
	aie::Input::getInstance()->getMouseXY(&mx, &my);
	return (my < m_zonePanelY || my < m_buildingPanelY);
}

void UiManager::drawBuildingPanel(aie::Renderer2D* renderer)
{
	renderer->setRenderColour(m_panelColour);
	renderer->drawBox(m_game->getWindowWidth() / 2.0f, m_buildingPanelY / 2.0f,
		(float)m_game->getWindowWidth(), m_buildingPanelY);

	// box behind panel title
	renderer->drawBox(32, m_buildingPanelY, 136, 24);
	// panel title
	renderer->setRenderColour(1, 1, 1);
	renderer->drawText(m_game->m_uiFontLarge, "Buildings", 8, m_buildingPanelY - 8);

	// draw all the boxes
	for (int i = 0; i < BUILDINGTYPE_COUNT; i++)
	{
		Rect thisRect = m_buildingBoxes[i];

		float xPos = thisRect.x + thisRect.width / 2.0f;
		float yPos = m_buildingPanelY - (thisRect.y - thisRect.height / 2.0f);

		// draw back rect
		if (m_game->getBuildingManager()->getSelectedBuilding() == i)
			renderer->setRenderColour(0.6f, 0.6f, 0.6f);
		else
			renderer->setRenderColour(0.1f, 0.1f, 0.1f);
		renderer->drawBox(xPos, yPos, thisRect.width, thisRect.height);

		// draw building icon
		renderer->setRenderColour(1, 1, 1);
		renderer->drawSprite(m_buildingIcons[i], xPos, yPos + thisRect.height / 6.0f);

		// draw building name
		float stringWidth = m_game->m_uiFont->getStringWidth(m_buildingNames[i]);
		renderer->drawText(m_game->m_uiFont, m_buildingNames[i], xPos - stringWidth / 2.0f,
			yPos - thisRect.height / 2.5f);
	}
}

void UiManager::drawZonePanel(aie::Renderer2D* renderer)
{
	renderer->setRenderColour(m_panelColour);
	renderer->drawBox(m_game->getWindowWidth() / 2.0f, m_zonePanelY / 2.0f,
		(float)m_game->getWindowWidth(), m_zonePanelY);

	// box behind panel title
	renderer->drawBox(32, m_zonePanelY, 82, 24);
	// panel title
	renderer->setRenderColour(1, 1, 1);
	renderer->drawText(m_game->m_uiFontLarge, "Zones", 8, m_zonePanelY - 8);

	// draw all the boxes
	for (int i = 0; i < ZONETYPE_COUNT; i++)
	{
		Rect thisRect = m_zoneBoxes[i];

		float xPos = thisRect.x + thisRect.width / 2.0f;
		float yPos = m_zonePanelY - (thisRect.y - thisRect.height / 2.0f);

		// draw back rect
		if (m_game->getZoneManager()->getSelectedType() == i)
			renderer->setRenderColour(0.6f, 0.6f, 0.6f);
		else
			renderer->setRenderColour(0.1f, 0.1f, 0.1f);
		renderer->drawBox(xPos, yPos, thisRect.width, thisRect.height);

		// draw coloured box indicating which type of zone
		renderer->setRenderColour(m_zoneColours[i]);
		renderer->drawBox(xPos, yPos + thisRect.height / 6.0f, thisRect.width / 2.0f,
			thisRect.width / 2.0f);

		// draw the X from the demolition icon for de-zone
		if (i == ZONETYPE_NONE)
		{
			renderer->setRenderColour(1, 1, 1, 1);
			renderer->drawSprite(m_game->getImageManager()->getTexture("icons/demolish"), xPos,
				yPos + thisRect.height / 6.0f);
		}

		// draw zone name text
		renderer->setRenderColour(1, 1, 1);
		float stringWidth = m_game->m_uiFont->getStringWidth(m_zoneNames[i]);
		renderer->drawText(m_game->m_uiFont, m_zoneNames[i], xPos - stringWidth / 2.0f,
			yPos - thisRect.height / 2.5f);
	}
}

bool UiManager::isMouseInRect(Rect r, float yoffset)
{
	int mx, my;
	aie::Input::getInstance()->getMouseXY(&mx, &my);

	float yPos = r.y - yoffset;

	if (mx > r.x && mx < r.x + r.width
		&& my > yPos - r.height / 2.0f && my < yPos + r.height*2.0f)
		return true;
	return false;
}
