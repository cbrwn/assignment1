#include "game.h"
#include "road.h"

Road::Road(Game* game, int x, int y)
	: Building(game, x, y)
{
	m_texture = m_game->getImageManager()->getTexture("buildings/road_right");

	m_type = BUILDINGTYPE_ROAD;
	m_buildStyle = BUILDSTYLE_LINE;
}

void Road::draw(aie::Renderer2D* renderer)
{
	if (m_posX < 0 || m_posY < 0)
		return;

	const float xOrigin = 0.5f;
	const float yOrigin = 0.0f;
	renderer->drawSprite(m_texture, m_worldPos.getX(), 
		m_worldPos.getY() - 4 + m_altitude, 0, 0, 0, 0, xOrigin, yOrigin);
}

// used for sorting
// turns the indices used in a 2 dimensional array (x, y)
//   into a single index as if it was in a 1D array
int Road::getOneDimensionalIndex()
{
	return (m_posY * WORLD_WIDTH) + m_posX;
}
