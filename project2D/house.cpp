#include "house.h"
#include "game.h"
#include "imagemanager.h"

House::House(Game* game, int x, int y)
	: Building(game, x, y)
{
	m_type = BuildingType::BUILDINGTYPE_HOUSE;
	m_buildStyle = BuildStyle::BUILDSTYLE_FORBIDDEN;
	m_sizeX = 1;
	m_sizeY = 1;
	m_powerSearchRange = 1;
	m_powerSpreadRange = 0;
	m_shakesCamera = false;

	m_texture = m_game->getImageManager()->getTexture("buildings/house");
}

void House::draw(aie::Renderer2D* renderer)
{
	if (m_posX < 0 || m_posY < 0)
		return;

	const float xOrigin = 0.5f;
	const float yOrigin = 0.0f;
	renderer->drawSprite(m_texture, m_worldPos.getX(),
		m_worldPos.getY() + m_altitude - 3, 0, 0, 0, 0, xOrigin, yOrigin);

	// eyeballs
	drawEyeball(renderer, Vector2(m_worldPos.getX() - 44,
		m_worldPos.getY() + m_altitude + 48.0f), 12.0f);
	drawEyeball(renderer, Vector2(m_worldPos.getX() - 18,
		m_worldPos.getY() + m_altitude + 34.0f), 12.0f);
}
