#pragma once

#include "tile.h"

class TileShop : public Tile
{
public:
	TileShop(Game* game, bool right);

	virtual void  update();
	virtual char* getMouseoverText() override;

	void addEmployees(int amount);
	int  getEmployees() { return m_employees; }
private:
	int m_employees, m_maxEmployees;
};