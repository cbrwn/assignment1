////////////////////////////////////////////
// Building
// Base class for all buildings which can be
//    placed in the world
////////////////////////////////////////////

#include "building.h"
#include "game.h"

Building::Building(Game* game, int x, int y)
	: m_game(game), m_posX(x), m_posY(y)
{
	// default values
	m_sizeX = 1;
	m_sizeY = 1;
}
