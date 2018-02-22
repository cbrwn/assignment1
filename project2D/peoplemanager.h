#pragma once

#include <vector>

// the rate of updating in seconds
#define PEOPLE_UPDATE_RATE (1)

class Game;
class TileHouse;
class TileShop;

class PeopleManager
{
public:
	PeopleManager(Game* game);

	void update(float delta);

	int getTotalPopulation() { return m_totalPopulation; }
	int getUnemployed() { return m_unemployedPopulation; }
private:
	Game* m_game;

	// keep track of the time since the last update
	float					m_timeSinceUpdate;

	std::vector<TileHouse*>	m_allHouses;
	std::vector<TileShop*>  m_allShops;
	int						m_totalPopulation;
	int						m_unemployedPopulation;

	void updateShops();

	int updateTotalPopulation();
	int updateUnemployedPopulation();
};