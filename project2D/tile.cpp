#include <Renderer2D.h>

#include "game.h"
#include "tile.h"

Tile::Tile(Game* game, aie::Texture* tex) :
	m_game(game), m_texture(tex)
{
	m_tileType = TileType::GRASS;
}

void Tile::draw(aie::Renderer2D * renderer, float dx, float dy)
{
	renderer->drawSprite(m_texture, dx, dy);
}

void Tile::update()
{
}

char* Tile::getMouseoverText()
{
	return "";
}

aie::Texture* Tile::getTexture()
{
	return m_texture;
}

void Tile::setTexture(aie::Texture* tex)
{
	m_texture = tex;
}

TileType Tile::getType()
{
	return m_tileType;
}

void Tile::setIndices(int ix, int iy)
{
	m_ix = ix;
	m_iy = iy;
}

// stores the tile's place in the array into ix and iy
void Tile::getIndices(int* ix, int* iy)
{
	*ix = m_ix;
	*iy = m_iy;
}
