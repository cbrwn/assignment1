////////////////////////////////////////////
// UiManager
// Base class for all buildings which can be
//    placed in the world
////////////////////////////////////////////

#include <Input.h>
#include <Font.h>

#include "game.h"
#include "uimanager.h"

#include "tile.h"

UiManager::UiManager(Game* game) : m_game(game), m_panelColour(0x444444ff)
{
	m_panelHeight = 100.0f;
}

UiManager::~UiManager()
{
}

void UiManager::update(float delta)
{
}

void UiManager::draw(aie::Renderer2D* renderer)
{
	// base panel box
	renderer->setRenderColour(m_panelColour);
	renderer->drawBox(m_game->getWindowWidth() / 2.0f, m_panelHeight / 2.0f,
		(float)m_game->getWindowWidth(), m_panelHeight);

	// box behind panel title
	renderer->drawBox(32, m_panelHeight, 64, 24);
	// panel title
	renderer->setRenderColour(1, 1, 1);
	renderer->drawText(m_game->m_uiFontLarge, "Build", 8, m_panelHeight - 8);

	aie::Input* input = aie::Input::getInstance();
	int mx, my;
	input->getMouseXY(&mx, &my);
}

bool UiManager::isMouseOverUi()
{
	int mx, my;
	aie::Input::getInstance()->getMouseXY(&mx, &my);
	return my < m_panelHeight;
}