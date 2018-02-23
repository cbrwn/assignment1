#include <Renderer2D.h>

#include "game.h"
#include "tile.h"

Tile::Tile(Game* game, aie::Texture* tex) :
	m_game(game), m_texture(tex)
{
}

void Tile::draw(aie::Renderer2D * renderer, float dx, float dy)
{
	renderer->drawSprite(m_texture, dx, dy, 0, 0, 0, 0, 0, 0.5f);
}

void Tile::update()
{
}

aie::Texture* Tile::getTexture()
{
	return m_texture;
}

void Tile::setTexture(aie::Texture* tex)
{
	m_texture = tex;
}