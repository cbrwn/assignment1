#include "Input.h"
#include "Font.h"

#include "game.h"
#include "imagemanager.h"
#include "uimanager.h"
#include "buildingmanager.h"
#include "tilemanager.h"

#include "tile.h"

UiManager::UiManager(Game* game)
	: m_game(game), m_panelColour(0x444444ff), m_panelY(120.0f)
{
	m_shownPanel = 0;

	m_buildingPanelDestY = -50;
	m_buildingPanelY = m_buildingPanelDestY;
	m_zonePanelDestY = -50;
	m_zonePanelY = m_zonePanelDestY;

	m_demandGraphPos.setX((float)m_game->getWindowWidth() - 32.0f);
	m_demandGraphPos.setY((float)m_game->getWindowHeight() - 64.0f);
	m_mouseOverGraph = false;

	m_resDemand = 0.0f;
	m_comDemand = 0.0f;
	m_indDemand = 0.0f;

	m_moneyFlashTime = 0.0f;

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
		// skip house, not buildable
		if (i == BUILDINGTYPE_HOUSE
			|| i == BUILDINGTYPE_SHOP
			|| i == BUILDINGTYPE_FACTORY)
		{
			m_buildingBoxes[i] = { 0,0,-1,-1 };
			continue;
		}

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
	m_buildingIcons[BUILDINGTYPE_NONE] =
		img->getTexture("icons/demolish");
	m_buildingIcons[BUILDINGTYPE_POWERPLANT] =
		img->getTexture("icons/powerplant");
	m_buildingIcons[BUILDINGTYPE_POWERPOLE] =
		img->getTexture("icons/powerpole");
	m_buildingIcons[BUILDINGTYPE_ROAD] =
		img->getTexture("icons/road");

	m_buildingNames[BUILDINGTYPE_NONE] = "Demolish";
	m_buildingNames[BUILDINGTYPE_POWERPLANT] = "Power Plant";
	m_buildingNames[BUILDINGTYPE_POWERPOLE] = "Power Pole";
	m_buildingNames[BUILDINGTYPE_ROAD] = "Road";

	m_buildingSelectorIcon = img->getTexture("icons/building");
	m_zoneSelectorIcon = img->getTexture("icons/zone");
}

UiManager::~UiManager()
{
}

void UiManager::update(float delta)
{
	const float smoothSpeed = 10.0f;
	m_buildingPanelY -= (m_buildingPanelY - m_buildingPanelDestY) *
		delta * smoothSpeed;
	m_zonePanelY -= (m_zonePanelY - m_zonePanelDestY) * delta * smoothSpeed;

	if (m_moneyFlashTime > 0.0f)
		m_moneyFlashTime -= delta;

	// do input
	aie::Input* input = aie::Input::getInstance();
	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT)
		&& isMouseOverUi())
	{
		int mx, my;
		input->getMouseXY(&mx, &my);

		// check for zone panel clickage when it's visible
		if (m_zonePanelY >= 0.0f)
			for (int i = 0; i < ZONETYPE_COUNT; i++)
				if (isMouseInRect(m_zoneBoxes[i], m_zonePanelY))
				{
					m_game->getTileManager()->setSelectedType((ZoneType)i);
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

		// check for selector box clicks
		if (isMouseInRect(m_selectorBox, 0, true))
		{
			if (mx < m_selectorBox.x) // left side was clicked
				selectorBoxClicked(UI_PANEL_BUILDINGS);
			else // right side was clicked
				selectorBoxClicked(UI_PANEL_ZONES);
		}
	}

	// check to see if mouse is over the demand graph
	// so we can show the mouseover box to explain it
	Rect demandGraphRect = {
		m_demandGraphPos.getX() - 10.0f,
		m_demandGraphPos.getY() - 10.0f,
		39.0f, // bad hardcoded size of the demand graph
		24.0f
	};
	m_mouseOverGraph = isMouseInRect(demandGraphRect, 0.0f);

	// make sure the selector box is always at the top of the screen
	m_selectorBox.y = m_game->getWindowHeight() - m_selectorBox.height / 2.0f;

	// smooth the demand values
	const float demandSmoothSpeed = 10.0f;
	BuildingManager* bm = m_game->getBuildingManager();
	m_resDemand -= (m_resDemand - bm->getDemand(ZONETYPE_RESIDENTIAL))
		* delta * demandSmoothSpeed;
	m_comDemand -= (m_comDemand - bm->getDemand(ZONETYPE_COMMERCIAL))
		* delta * demandSmoothSpeed;;
	m_indDemand -= (m_indDemand - bm->getDemand(ZONETYPE_INDUSTRIAL))
		* delta * demandSmoothSpeed;;
}

void UiManager::draw(aie::Renderer2D* renderer)
{
	// only want to draw the panels if they're on screen
	if (m_buildingPanelY >= 0.0f)
		drawBuildingPanel(renderer);
	if (m_zonePanelY >= 0.0f)
		drawZonePanel(renderer);

	drawDemandGraph(renderer);

	// draw selector box
	renderer->setRenderColour(m_panelColour);
	renderer->drawBox(m_selectorBox.x, m_selectorBox.y, m_selectorBox.width,
		m_selectorBox.height);
	renderer->setRenderColour(m_panelColour - 0x33333333);
	renderer->drawLine(m_selectorBox.x,
		m_selectorBox.y - m_selectorBox.height / 3.0f, m_selectorBox.x,
		m_selectorBox.y + m_selectorBox.height / 3.0f);
	// selector icons
	renderer->setRenderColour(1, 1, 1);
	float buildingIconX = m_selectorBox.x - (m_selectorBox.width / 4.0f);
	renderer->drawSprite(m_buildingSelectorIcon,
		buildingIconX, m_selectorBox.y);
	float zoneIconX = m_selectorBox.x + (m_selectorBox.width / 4.0f);
	renderer->drawSprite(m_zoneSelectorIcon, zoneIconX, m_selectorBox.y);

	if (m_mouseOverGraph)
		drawDemandMouseover(renderer);

	// show money
	aie::Font* moneyFont = m_game->m_uiFontLarge;
	char mny[32];
	sprintf_s(mny, 32, "$%d", m_game->getMoney());

	float moneyWidth = moneyFont->getStringWidth(mny);
	if (m_moneyFlashTime > 0 && (int)(m_moneyFlashTime * 10) % 2 == 0)
		renderer->setRenderColour(0.8f, 0, 0);
	else
		renderer->setRenderColour(0, 0.4f, 0);
	renderer->drawText(moneyFont, mny,
		m_game->getWindowWidth() - moneyWidth - 2,
		m_game->getWindowHeight() - 18.0f);
}

void UiManager::setShownPanel(int panel)
{
	// set the destination to below 0 to hide the whole panel 
	//    (including the title)
	const float hiddenValue = -50.0f;
	m_shownPanel = panel;
	m_buildingPanelDestY = panel == UI_PANEL_BUILDINGS ? m_panelY :
		hiddenValue;
	m_zonePanelDestY = panel == UI_PANEL_ZONES ? m_panelY : hiddenValue;
}

bool UiManager::isMouseOverUi()
{
	if (isMouseInRect(m_selectorBox, 0, true))
		return true;
	int mx, my;
	aie::Input::getInstance()->getMouseXY(&mx, &my);
	return (my < m_zonePanelY || my < m_buildingPanelY);
}

void UiManager::flashMoney()
{
	if (m_moneyFlashTime <= 0.0f)
		m_moneyFlashTime = 1.0f;
}

void UiManager::drawBuildingPanel(aie::Renderer2D* renderer)
{
	renderer->setRenderColour(m_panelColour);
	renderer->drawBox(m_game->getWindowWidth() / 2.0f,
		m_buildingPanelY / 2.0f, (float)m_game->getWindowWidth(),
		m_buildingPanelY);

	// box behind panel title
	renderer->drawBox(32, m_buildingPanelY, 136, 24);
	// panel title
	renderer->setRenderColour(1, 1, 1);
	renderer->drawText(m_game->m_uiFontLarge, "Buildings", 8,
		m_buildingPanelY - 8);

	// draw all the boxes
	for (int i = 0; i < BUILDINGTYPE_COUNT; i++)
	{
		// skip boxes for buildings that aren't buildable
		if (m_buildingBoxes[i].width < 0)
			continue;

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
		renderer->drawSprite(m_buildingIcons[i], xPos,
			yPos + thisRect.height / 6.0f);

		// draw building name
		float nameWidth = m_game->m_uiFont->getStringWidth(m_buildingNames[i]);
		renderer->drawText(m_game->m_uiFont, m_buildingNames[i],
			xPos - nameWidth / 2.0f, yPos - thisRect.height / 2.5f);
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
		if (m_game->getTileManager()->getSelectedType() == i)
			renderer->setRenderColour(0.6f, 0.6f, 0.6f);
		else
			renderer->setRenderColour(0.1f, 0.1f, 0.1f);
		renderer->drawBox(xPos, yPos, thisRect.width, thisRect.height);

		// draw coloured box indicating which type of zone
		renderer->setRenderColour(m_zoneColours[i]);
		renderer->drawBox(xPos, yPos + thisRect.height / 6.0f,
			thisRect.width / 2.0f, thisRect.width / 2.0f);

		// draw the X from the demolition icon for de-zone
		if (i == ZONETYPE_NONE)
		{
			renderer->setRenderColour(1, 1, 1, 1);
			renderer->drawSprite(m_buildingIcons[0], xPos,
				yPos + thisRect.height / 6.0f);
		}

		// draw zone name text
		renderer->setRenderColour(1, 1, 1);
		float stringWidth = m_game->m_uiFont->getStringWidth(m_zoneNames[i]);
		renderer->drawText(m_game->m_uiFont, m_zoneNames[i],
			xPos - stringWidth / 2.0f, yPos - thisRect.height / 2.5f);
	}
}

void UiManager::drawDemandGraph(aie::Renderer2D* renderer)
{
	const float columnWidth = 10.0f;
	const float barWidth = 0.8f * columnWidth;

	// stick these values in arrays to make drawing super easy
	const unsigned int demandColours[3] = {
		0x00ff00ff,
		0x0000ffff,
		0xffff00ff
	};
	float demandValues[3] = {
		m_resDemand,
		m_comDemand,
		m_indDemand
	};
	const char demandInitials[3] = {
		'R','C','I'
	};

	// box to put behind graph
	float boxWidth = columnWidth * 3.0f + barWidth / 2.0f;
	float boxHeight = 56.0f;
	renderer->setRenderColour(0.4f, 0.4f, 0.4f);
	renderer->drawBox(m_demandGraphPos.getX() + boxWidth / 2.0f - barWidth + 1,
		m_demandGraphPos.getY() + boxHeight / 2.0f - 16.0f,
		boxWidth, boxHeight);

	for (int i = 0; i < 3; i++)
	{
		ZoneType type = (ZoneType)(i + 1);
		float zoneDemand = demandValues[i] - 1.0f;
		zoneDemand *= 9.0f;

		float boxX = m_demandGraphPos.getX() + i * columnWidth;
		float boxY = m_demandGraphPos.getY();

		renderer->setRenderColour(demandColours[i]);
		renderer->drawBox(boxX, boxY + zoneDemand / 2.0f, barWidth, zoneDemand);

		// move the I a little to the right so it looks centered since it's
		//   so thin
		if (demandInitials[i] == 'I')
			boxX += 2.0f;

		renderer->setRenderColour(1, 1, 1);
		// turn the initial to a null-terminated string
		char initial[2] = { demandInitials[i], 0 };
		renderer->drawText(m_game->m_uiFont, initial, boxX - 4, boxY - 12.0f);
	}
}

void UiManager::drawDemandMouseover(aie::Renderer2D* renderer)
{
	const char text[32] = "Demand for each type of zone";

	// grab all the values we'll use to draw the box
	const float textWidth = m_game->m_uiFont->getStringWidth(text);
	const float textHeight = m_game->m_uiFont->getStringHeight(text);

	const float boxPadding = 4.0f;
	const float boxWidth = textWidth + boxPadding * 2.0f;
	const float boxHeight = textHeight + boxPadding * 2.0f;

	const float boxX = m_demandGraphPos.getX() - boxWidth / 2.0f + 28.0f;
	const float boxY = m_demandGraphPos.getY() + boxHeight / 2.0f - 38.0f;

	const float textX = boxX - textWidth / 2.0f;
	const float textY = boxY - textHeight / 4.0f;

	// now we can draw everything!
	renderer->setRenderColour(0.3f, 0.3f, 0.3f, 0.8f);
	renderer->drawBox(boxX, boxY, boxWidth, boxHeight);

	renderer->setRenderColour(1, 1, 1);
	renderer->drawText(m_game->m_uiFont, text, textX, textY);
}

// toggles the panel clicked on using the selector box
void UiManager::selectorBoxClicked(int panel)
{
	if (m_shownPanel == panel)
		panel = 0; // close panel

	m_game->setPlaceMode((PlaceMode)panel);
	setShownPanel(panel);
}

bool UiManager::isMouseInRect(Rect r, float yoffset, bool centerOrigin)
{
	int mx, my;
	aie::Input::getInstance()->getMouseXY(&mx, &my);

	// adjust rectangle if the origin of the rect is the center
	if (centerOrigin)
		r.x -= r.width / 2.0f;

	float yPos = r.y - yoffset;

	if (mx > r.x && mx < r.x + r.width
		&& my > yPos - r.height / 2.0f && my < yPos + r.height*2.0f)
		return true;
	return false;
}
