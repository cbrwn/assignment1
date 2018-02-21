#include <iostream>

#include "game.h"
#include "tile_house.h"

TileHouse::TileHouse(Game* game, bool right)
	: Tile(game, game->m_imageManager->getTexture("tiles/house_left"))
{
	m_tileType = TileType::HOUSE;
	if (right)
		setTexture(game->m_imageManager->getTexture("tiles/house_right"));

	m_people = 0;
	m_capacity = 5;
}

void TileHouse::update()
{
}

char* TileHouse::getMouseoverText()
{
	char* result = new char[64];
	sprintf_s(result, 64, "People: %d / %d", m_people, m_capacity);
	return result;
}

void TileHouse::addPeople(int newPeople)
{
	m_people += newPeople;
	// limit to capacity
	if (m_people > m_capacity)
		m_people = m_capacity;
	// and make sure it's not going below 0
	if (m_people < 0)
		m_people = 0;
}
