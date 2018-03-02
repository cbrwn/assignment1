////////////////////////////////////////////
// Building
// Base class for all buildings which can be
//    placed in the world
////////////////////////////////////////////

#include "building.h"
#include "buildingmanager.h"
#include "powerpole.h"
#include "tile.h"
#include "game.h"

Building::Building(Game* game, int x, int y)
	: m_game(game), m_posX(x), m_posY(y)
{
	// default values
	m_sizeX = 1;
	m_sizeY = 1;
	m_buildStyle = BuildStyle::BUILDSTYLE_SINGLE;
	m_hasPower = false;
	m_powerSpreadRange = 2;
	m_powerSearchRange = 1;
	m_producesPower = false;

	// grab the world position
	m_game->getTileWorldPosition(m_posX + 1, m_posY, &m_worldX, &m_worldY);
}

void Building::update()
{
}

void Building::setPosition(int x, int y)
{
	m_posX = x;
	m_posY = y;

	// update world positions
	m_game->getTileWorldPosition(m_posX + 1, m_posY, &m_worldX, &m_worldY);
}

bool Building::updatePower()
{
	// check if a tile of ours has power
	int maxX = m_posX + m_powerSearchRange;
	int maxY = m_posY + m_powerSearchRange;
	int minX = m_posX - (m_sizeX - 1) - m_powerSearchRange;
	int minY = m_posY - (m_sizeY - 1) - m_powerSearchRange;

	m_hasPower = m_producesPower;
	if (!m_hasPower)
	{
		// if we have no power, search for it!
		for (int y = minY; y <= maxY; ++y)
		{
			for (int x = minX; x <= maxX; ++x)
			{
				Tile* thisTile = m_game->getTile(x, y);
				if (!thisTile)
					continue;
				if (thisTile->hasPower())
				{
					m_hasPower = true;
					break;
				}
			}
			if (m_hasPower)
				break;
		}
	}

	// don't continue with this if this building has no power
	if (!m_hasPower)
		return false;

	// we want to return true if this changed the state of the power
	bool gavePower = false;

	// make values larger for spreading power
	maxX = m_posX + m_powerSpreadRange;
	maxY = m_posY + m_powerSpreadRange;
	minX = m_posX - (m_sizeX - 1) - m_powerSpreadRange;
	minY = m_posY - (m_sizeY - 1) - m_powerSpreadRange;

	// pass power on to the next few tiles
	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			Tile* thisTile = m_game->getTile(x, y);
			if (!thisTile)
				continue;
			if (!thisTile->hasPower())
			{
				gavePower = true;
				thisTile->givePower();
			}
		}
	}

	return gavePower;
}
