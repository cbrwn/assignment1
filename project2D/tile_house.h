#pragma once

#include "tile.h"

class TileHouse : public Tile
{
public:
	TileHouse(Game* game, bool right);

	virtual void update();
	
	virtual char* getMouseoverText() override;

	void setCapacity(int capacity) { m_capacity = capacity; }
	int getCapacity() { return m_capacity; }

	void setPeople(int people) { m_people = people; }
	int getPeople() { return m_people; }
	void addPeople(int newPeople);
private:
	int m_capacity, m_people;
};