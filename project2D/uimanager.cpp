////////////////////////////////////////////
// UiManager
// Base class for all buildings which can be
//    placed in the world
////////////////////////////////////////////

#include <Input.h>
#include <Font.h>

#include "game.h"
#include "uimanager.h"
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
		for (int i = 0; i < ZONETYPE_COUNT; i++)
		{
			if (isMouseInRect(m_zoneBoxes[i], m_zonePanelY))
			{
				m_game->getZoneManager()->setSelectedType((ZoneType)i);
				break;
			}
		}
	}
}

void UiManager::draw(aie::Renderer2D* renderer)
{
	drawBuildingPanel(renderer);
	drawZonePanel(renderer);
}

void UiManager::setShownPanel(int panel)
{
	// set the destination to below 0 to hide the whole panel
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

	const unsigned int zoneColours[4] = {
		0x00000000, // none
		0x00ff00ff, // residential - green
		0x0000ffff, // commercial - blue
		0xffff00ff  // industrial - yellow
	};

	const char* zoneNames[4] = {
		"De-zone",
		"Residential",
		"Commercial",
		"Industrial"
	};

	// draw all the boxes
	for (int i = 0; i < ZONETYPE_COUNT; i++)
	{
		Rect thisRect = m_zoneBoxes[i];

		float xPos = thisRect.left + thisRect.width / 2.0f;
		float yPos = m_zonePanelY - (thisRect.top - thisRect.height / 2.0f);

		if(m_game->getZoneManager()->getSelectedType() == i)
			renderer->setRenderColour(0.6f, 0.6f, 0.6f);
		else
			renderer->setRenderColour(0.1f, 0.1f, 0.1f);
		renderer->drawBox(xPos, yPos, thisRect.width, thisRect.height);

		renderer->setRenderColour(zoneColours[i]);
		renderer->drawBox(xPos, yPos + thisRect.height / 6.0f, thisRect.width / 2.0f, 
			thisRect.width / 2.0f);

		renderer->setRenderColour(1, 1, 1);
		float stringWidth = m_game->m_uiFont->getStringWidth(zoneNames[i]);
		renderer->drawText(m_game->m_uiFont, zoneNames[i], xPos - stringWidth/2.0f, 
			yPos - thisRect.height / 2.5f);
	}
}

bool UiManager::isMouseInRect(Rect r, float yoffset)
{
	int mx, my;
	aie::Input::getInstance()->getMouseXY(&mx, &my);

	float yPos = r.top - yoffset;

	if (mx > r.left && mx < r.left + r.width 
		&& my > yPos - r.height/2.0f && my < yPos + r.height*2.0f)
		return true;
	return false;
}
