////////////////////////////////////////////
// Tile
// Basic class to represent each tile of
//   the map
////////////////////////////////////////////

#include <Renderer2D.h>

#include "game.h"
#include "tile.h"

unsigned int Tile::m_zoneTintColours[ZONETYPE_COUNT];

Tile::Tile(Game* game, aie::Texture* tex) :
	m_game(game), m_texture(tex)
{
	m_zoneType = ZONETYPE_NONE;

	// set the tints for each zone type
	m_zoneTintColours[ZONETYPE_NONE] = 0xffffffff;
	m_zoneTintColours[ZONETYPE_RESIDENTIAL] = 0x88ff88ff;
	m_zoneTintColours[ZONETYPE_COMMERCIAL] = 0x4488ffff;
	m_zoneTintColours[ZONETYPE_INDUSTRIAL] = 0xffbb00ff;
}

void Tile::draw(aie::Renderer2D * renderer, float dx, float dy, bool zoneTint)
{
	if (zoneTint)
		renderer->setRenderColour(m_zoneTintColours[getZoneType()]);
	renderer->drawSprite(m_texture, dx, dy, 0, 0, 0, 0, 0, 0.5f);
}

void Tile::update() { }

aie::Texture* Tile::getTexture()
{
	return m_texture;
}

void Tile::setTexture(aie::Texture* tex)
{
	m_texture = tex;
}

void Tile::setZoneType(ZoneType type)
{
	// handle destroying existing zone-specific stuff here
	m_zoneType = type;
}
