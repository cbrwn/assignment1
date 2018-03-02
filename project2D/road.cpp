#include "game.h"
#include "road.h"

Road::Road(Game* game, int x, int y)
	: Building(game, x, y)
{
	m_texture = m_game->getImageManager()->getTexture("buildings/road_right");

	m_type = BUILDINGTYPE_ROAD;
	m_buildStyle = BUILDSTYLE_LINE;

	cfield = 0;
}

void Road::draw(aie::Renderer2D* renderer)
{
	if (m_posX < 0 || m_posY < 0)
		return;

	const float xOrigin = 0.5f;
	const float yOrigin = 0.0f;
	renderer->drawSprite(m_texture, m_worldX, m_worldY - 4, 0, 0, 0, 0, xOrigin,
		yOrigin);
}

void Road::update()
{
	// roads don't need power which is updated in the building update
	//   so we do nothing here
}