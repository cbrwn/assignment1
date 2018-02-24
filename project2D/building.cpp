////////////////////////////////////////////
// Building
// Base class for all buildings which can be
//    placed in the world
////////////////////////////////////////////

#include "building.h"
#include "buildingmanager.h"
#include "powerpole.h"
#include "game.h"

Building::Building(Game* game, int x, int y)
	: m_game(game), m_posX(x), m_posY(y)
{
	// default values
	m_sizeX = 1;
	m_sizeY = 1;
	m_buildStyle = BuildStyle::SINGLE;
	m_hasPower = false;
}

void Building::update()
{
	m_hasPower = false;
	// check for powered power poles to determine if we have power
	for (Building* b : *(m_game->getBuildingManager()->getPowerPoles()))
	{
		if (!b->hasPower())
			continue;
		PowerPole* p = (PowerPole*)b;
		if (p->isTouchingBuilding(this))
		{
			m_hasPower = true;
			break;
		}
	}
}
