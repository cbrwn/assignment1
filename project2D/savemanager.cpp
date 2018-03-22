#include <iostream>
#include <fstream>

#include "savemanager.h"
#include "game.h"
#include "buildingmanager.h"
#include "roadmanager.h"
#include "building.h"
#include "tile.h"
#include "tilemanager.h"

SaveManager::SaveManager(Game* game)
	: m_game(game)
{
}

bool SaveManager::loadData()
{
	std::fstream file;
	file.open(SAVEFILE_NAME, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		printf("Couldn't open save file when loading!\n");
		return false;
	}

	int worldHeight, worldWidth;
	int buildingCount;

	// start reading!

	// read the world size
	file.read((char*)&worldWidth, 4);
	file.read((char*)&worldHeight, 4);
	
	// and building count
	file.read((char*)&buildingCount, 4);

	printf("Loading map of %dx%d tiles with %d buildings\n", 
		worldHeight, worldWidth, buildingCount);

	// read all tile types
	for (int y = 0; y < worldHeight; ++y)
	{
		for (int x = 0; x < worldWidth; ++x)
		{
			char zoneType;
			file.read(&zoneType, 1);
			m_game->getTileManager()->getTile(x, y)->setZoneType((ZoneType)zoneType);
		}
	}

	// clear buildings and pointers to any roads
	m_game->getRoadManager()->clearRoads();
	m_game->getBuildingManager()->clearBuildings();

	// randomly choose the direction to drop buildings in
	bool horz = (rand() % 100) < 50;

	// grab the buildings
	for (int i = 0; i < buildingCount; i++)
	{
		// building info we stored
		short buildingType;
		int buildingX, buildingY;

		// read em
		file.read((char*)&buildingType, 2);
		file.read((char*)&buildingX, 4);
		file.read((char*)&buildingY, 4);

		// make a new building with these values
		Building* build = m_game->getBuildingManager()->makeBuilding(
			(BuildingType)buildingType, buildingX, buildingY);

		// set altitude stuff to drop row-by-row
		int dropDir = buildingX;
		if (!horz)
			dropDir = buildingY;

		// I was going to make sure they're on the ground but this is
		//   more fun
		build->setAltitude(dropDir * 500.0f);

		m_game->getBuildingManager()->addBuilding(build, false);

		// make sure we keep track of roads!
		if (buildingType == BUILDINGTYPE_ROAD)
			m_game->getRoadManager()->addRoad(build, false);
	}

	// and sort/update everything!
	m_game->getBuildingManager()->sortBuildings();
	m_game->getRoadManager()->updateRoads();

	file.close();
	return true;
}

bool SaveManager::saveData()
{
	/*
		format stuff:
		8 bytes: size of tiles (even though it's constant for now)
			width
			height
		4 bytes: number of buildings
		=================================
		all tile info (zones)
		----------------------
		all buildings (x, y, type)
	*/
	std::fstream file;
	file.open(SAVEFILE_NAME, std::ios::out | std::ios::binary);

	if (!file.is_open())
	{
		printf("Couldn't open save file when saving!!\n");
		return false;
	}

	BuildingList* buildings = m_game->getBuildingManager()->getBuildings();

	int worldWidth = WORLD_WIDTH;
	int worldHeight = WORLD_HEIGHT;
	int buildingCount = (int)buildings->getCount();

	// start writing!

	// write world width and height
	file.write((char*)&worldWidth, 4);
	file.write((char*)&worldHeight, 4);

	// and now write the number of buildings
	file.write((char*)&buildingCount, 4);

	// write all the tiles' zones, 1 byte each
	for (int y = 0; y < worldHeight; ++y)
	{
		for (int x = 0; x < worldWidth; ++x)
		{
			Tile* thisTile = m_game->getTileManager()->getTile(x, y);
			char zoneType = thisTile->getZoneType();
			file.write((char*)&zoneType, 1);
		}
	}

	// and write all the buildings!
	// 2 bytes for type, 8 bytes for position
	for (int i = 0; i < buildingCount; i++)
	{
		Building* thisBuilding = (*buildings)[i];
		short buildingType = thisBuilding->getType();
		int buildingX, buildingY;
		thisBuilding->getPosition(&buildingX, &buildingY);

		// write these
		file.write((char*)&buildingType, 2); // building ID
		file.write((char*)&buildingX, 4); // x position
		file.write((char*)&buildingY, 4); // y position
	}

	file.close();
	return true;
}
