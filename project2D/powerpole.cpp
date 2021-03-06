#include "powerpole.h"

#include "game.h"
#include "imagemanager.h"

PowerPole::PowerPole(Game* game, int x, int y)
	: Building(game, x, y)
{
	m_type = BUILDINGTYPE_POWERPOLE;
	m_buildStyle = BUILDSTYLE_LINE;
	m_sizeX = 1;
	m_sizeY = 1;
	m_powerSearchRange = 3;
	m_powerSpreadRange = 3;

	m_price = 5;

	m_texture = m_game->getImageManager()->getTexture("buildings/powerpole");
}

void PowerPole::draw(aie::Renderer2D* renderer)
{
	if (m_posX < 0 || m_posY < 0)
		return;

	// keep origin at the bottom-middle of the sprite
	// (where it would be touching the ground)
	const float xOrigin = 0.5f;
	const float yOrigin = 0.0f;
	renderer->drawSprite(m_texture, m_worldPos.getX(),
		m_worldPos.getY() + m_altitude, 0, 0, 0, 0, xOrigin, yOrigin);
}