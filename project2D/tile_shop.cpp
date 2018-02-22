#include <cstdlib>

#include "game.h"
#include "tile_shop.h"

TileShop::TileShop(Game* game, bool right)
	: Tile(game, game->getImageManager()->getTexture("tiles/shop_left"))
{
	m_tileType = TileType::SHOP;

	if (right)
		m_texture = game->getImageManager()->getTexture("tiles/shop_right");

	m_employees = 0;
	m_maxEmployees = 2;
}

void TileShop::update()
{
	// todo: base money making on the nearby population count?

	// randomize money made per employee
	int moneyMade = (rand() % 100);
	moneyMade *= m_employees;

	// moneyMade will be 0 if there are no employees which makes sense
	m_game->addMoney(moneyMade);
}

char* TileShop::getMouseoverText()
{
	char* result = new char[64];
	sprintf_s(result, 64, "Employees: %d / %d", m_employees, m_maxEmployees);
	return result;
}

void TileShop::addEmployees(int amount)
{
	// todo: check if this is more than the number of unemployed people
	m_employees += amount;
	// limit at maxEmployees
	if (m_employees > m_maxEmployees)
		m_employees = m_maxEmployees;
	// and make sure it's not below 0
	if (m_employees < 0)
		m_employees = 0;
}

