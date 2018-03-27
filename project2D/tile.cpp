#include "tile.h"

#include "Renderer2D.h"

#include "game.h"
#include "roadmanager.h"

unsigned int Tile::m_zoneTintColours[ZONETYPE_COUNT];

Tile::Tile(Game* game, aie::Texture* tex) :
	m_game(game), m_texture(tex)
{
	m_xIndex = -1;
	m_yIndex = -1;

	m_building = nullptr;
	m_zoneType = ZONETYPE_NONE;
	m_hasPower = false;
	m_pollution = 0;

	// set the tints for each zone type
	m_zoneTintColours[ZONETYPE_NONE] = 0xffffffff;
	m_zoneTintColours[ZONETYPE_RESIDENTIAL] = 0x88ff88ff;
	m_zoneTintColours[ZONETYPE_COMMERCIAL] = 0x4488ffff;
	m_zoneTintColours[ZONETYPE_INDUSTRIAL] = 0xffbb00ff;
}

void Tile::draw(aie::Renderer2D* renderer, const float dx, 
	const float dy, const bool zoneTint)
{
	if (zoneTint)
		renderer->setRenderColour(m_zoneTintColours[getZoneType()]);
	renderer->drawSprite(m_texture, dx, dy, 0, 0, 0, 0, 0, 0.5f);
}

void Tile::setIndices(int x, int y)
{
	m_xIndex = x;
	m_yIndex = y;
}

void Tile::getIndices(int* x, int* y) const
{
	*x = m_xIndex;
	*y = m_yIndex;
}

// returns whether or not the tile is suitable for living
bool Tile::isLiveable() const
{
	if (!m_hasPower)
		return false;

	if (m_zoneType == ZONETYPE_NONE)
		return false;

	int roadDist;
	m_game->getRoadManager()->getClosestRoad(m_xIndex, m_yIndex, &roadDist);

	return roadDist <= 6;
}
