#include <iostream>

#include "peoplemanager.h"
#include "game.h"

#include "tile.h"
#include "tile_house.h"
#include "tile_shop.h"

PeopleManager::PeopleManager(Game* game)
	: m_game(game)
{
	m_totalPopulation = 0;
	m_timeSinceUpdate = 0;
	m_unemployedPopulation = 0;
}

void PeopleManager::update(float delta)
{
	m_timeSinceUpdate += delta;

	if (m_timeSinceUpdate >= PEOPLE_UPDATE_RATE)
	{
		// update our lists of buildings
		m_allHouses.clear();
		m_allShops.clear();
		for (int y = 0; y < WORLD_HEIGHT; y++)
		{
			for (int x = 0; x < WORLD_WIDTH; x++)
			{
				Tile* thisTile = m_game->m_tiles[y][x];
				if (thisTile != nullptr)
				{
					switch (thisTile->getType())
					{
					case TileType::HOUSE:
						m_allHouses.push_back((TileHouse*)thisTile);
						break;
					case TileType::SHOP:
						m_allShops.push_back((TileShop*)thisTile);
						break;
					}
				}
			}
		}

		// let's add a person to a house randomly every so often
		for (auto h : m_allHouses)
		{
			if ((rand() % 100) < 5)
				h->addPeople(1);
		}

		updateTotalPopulation();
		updateUnemployedPopulation();

		// process all shop-related stuff
		for (auto s : m_allShops)
		{
			// give people jobs!
			if (getUnemployed() > 0 && (rand() % 100) < 10)
				s->addEmployees(1);
			s->update();
		}

		// and let's make sure we don't have negative unemployment
		// which can happen if people disappear but a shop keeps them as an employee
		while (updateUnemployedPopulation() < 0)
		{
			for (auto s : m_allShops)
			{
				s->addEmployees(-1);
				if (updateUnemployedPopulation() >= 0)
					break;
			}
		}

		// update our record after we potentially give/take jobs
		updateUnemployedPopulation();

		m_timeSinceUpdate = 0;
	}
}

// updates our record of the total population and also returns it
int PeopleManager::updateTotalPopulation()
{
	int total = 0;
	// go through all houses and count the people
	for (auto h : m_allHouses)
		total += h->getPeople();
	m_totalPopulation = total;
	return total;
}

// updates our record of unemployed people and returns it
int PeopleManager::updateUnemployedPopulation()
{
	// count the number of employed people to subtract from total population
	int employed = 0;
	for (auto s : m_allShops)
		employed += s->getEmployees();
	m_unemployedPopulation = getTotalPopulation() - employed;
	return m_unemployedPopulation;
}


