#include "game.h"
#include "tile_military.h"

TileMilitary::TileMilitary(Game* game, bool right)
	: Tile(game, game->getImageManager()->getTexture("tiles/military_left"))
{
	if (right)
		setTexture(game->getImageManager()->getTexture("tiles/military_right"));

	m_middleLayer = game->getImageManager()->getTexture("tiles/military_roof_middle");

	if (right)
		m_roofTexture = game->getImageManager()->getTexture("tiles/military_roof_right");
	else
		m_roofTexture = game->getImageManager()->getTexture("tiles/military_roof_left");

	m_soldiers = 0;
	m_maxSoldiers = 5;

	m_tileType = TileType::MILITARY;
}

void TileMilitary::update()
{
}

void TileMilitary::draw(aie::Renderer2D* renderer, float dx, float dy)
{
	Tile::draw(renderer, dx, dy);
	// draw middle layer
	float middleY = dy + m_middleLayer->getHeight()/2.0f;
	renderer->drawSprite(m_middleLayer, dx, middleY);
	// and the roof
	middleY += (TILE_HEIGHT - m_roofTexture->getHeight()) / 2.0f;
	renderer->drawSprite(m_roofTexture, dx, middleY);
}

char* TileMilitary::getMouseoverText()
{
	char* txt = new char[64];
	sprintf_s(txt, 64, "Soldiers: %d / %d", m_soldiers, m_maxSoldiers);
	return txt;
}

void TileMilitary::addSoldiers(int amt)
{
	m_soldiers += amt;
	// limit soldier amount
	if (m_soldiers > m_maxSoldiers)
		m_soldiers = m_maxSoldiers;
	// and make sure we don't have negative soldiers!
	if (m_soldiers < 0)
		m_soldiers = 0;
}
