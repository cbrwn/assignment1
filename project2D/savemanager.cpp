#include "savemanager.h"

#include <iostream>

#include "game.h"
#include "tile.h"
#include "darray.h"
#include "building.h"
#include "roadmanager.h"
#include "tilemanager.h"
#include "buildingmanager.h"

SaveManager::SaveManager(Game* game)
	: m_game(game), m_headerSize(4 + 8 + 4 /* money + map size + building count */)
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

	// load money
	int tempMoney; // temp variable to store it in
	file.read((char*)&tempMoney, 4);
	m_game->setMoney(tempMoney); // then stick it into Game

	// read the world size
	file.read((char*)&worldWidth, 4);
	file.read((char*)&worldHeight, 4);

	// and building count
	file.read((char*)&buildingCount, 4);

	printf("Loading map of %dx%d tiles with %d buildings and $%d\n",
		worldHeight, worldWidth, buildingCount, tempMoney);

	// load all the tiles
	readTiles(&file, worldWidth, worldHeight);

	// read em
	readBuildings(&file, buildingCount);

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
		4 bytes: money
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

	// write money
	int tempMoney = m_game->getMoney(); // get it in a temporary variable first
	file.write((char*)&tempMoney, 4);

	// write world width and height
	file.write((char*)&worldWidth, 4);
	file.write((char*)&worldHeight, 4);

	// and now write the number of buildings
	file.write((char*)&buildingCount, 4);

	// write all the tiles
	writeTiles(&file, worldWidth, worldHeight);

	// and the buildings
	writeBuildings(&file);

	file.close();
	return true;
}

// used in loadTiles so we know how many tiles to load
void SaveManager::getWorldSize(int* width, int* height, std::fstream* openFile)
{
	// double check that the file is open
	if (!openFile->is_open())
	{
		*width = -1;
		*height = -1;
		return;
	}

	// start of file is MONEY, WORLD WIDTH, WORLD HEIGHT, BUILDING COUNT
	openFile->seekg(4, std::ios::beg); // skip past the money value
	int tempWidth, tempHeight;
	openFile->read((char*)&tempWidth, 4);
	openFile->read((char*)&tempHeight, 4);

	*width = tempWidth;
	*height = tempHeight;
}

int SaveManager::getBuildingCount(std::fstream* openFile)
{
	// double check that the file is open
	if (!openFile->is_open())
		return -1;

	// start of file is MONEY, WORLD WIDTH, WORLD HEIGHT, BUILDING COUNT
	openFile->seekg(12, std::ios::beg); // skip past money and world size
	int count;
	openFile->read((char*)&count, 4);

	return count;
}

// reads tiles and sets them in the game, assuming the read
// pointer is in the correct position
void SaveManager::readTiles(std::fstream* openFile, int worldWidth, int worldHeight)
{
	// make sure tile list is fresh and sized properly
	m_game->getTileManager()->clearTiles(worldWidth, worldHeight);

	// read all tile zones
	for (int y = 0; y < worldHeight; ++y)
	{
		for (int x = 0; x < worldWidth; ++x)
		{
			char zoneType;
			openFile->read(&zoneType, 1);
			m_game->getTileManager()->getTile(x, y)->setZoneType((ZoneType)zoneType);
		}
	}
}

// reads buildings and adds them to the game, assuming the read
// pointer is in the correct position
void SaveManager::readBuildings(std::fstream* openFile, int buildingCount)
{
	// clear buildings and pointers to any roads before loading
	m_game->getRoadManager()->clearRoads();
	m_game->getBuildingManager()->clearBuildings();

	// randomly choose the direction to drop buildings in
	bool horz = (rand() % 100) < 50;

	for (int i = 0; i < buildingCount; ++i)
	{
		// building info we stored
		short buildingType;
		int buildingX, buildingY;

		// read em
		openFile->read((char*)&buildingType, 2);
		openFile->read((char*)&buildingX, 4);
		openFile->read((char*)&buildingY, 4);

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
	}
}

// writes tiles to openFile, assuming the write 
// pointer is in the correct position
void SaveManager::writeTiles(std::fstream* openFile, int worldWidth, int worldHeight)
{
	// write all the tiles' zones, 1 byte each
	for (int y = 0; y < worldHeight; ++y)
	{
		for (int x = 0; x < worldWidth; ++x)
		{
			Tile* thisTile = m_game->getTileManager()->getTile(x, y);
			char zoneType = thisTile->getZoneType();
			openFile->write((char*)&zoneType, 1);
		}
	}
}

void SaveManager::writeBuildings(std::fstream* openFile)
{
	BuildingList* buildings = m_game->getBuildingManager()->getBuildings();
	int buildingCount = buildings->getCount();

	// 2 bytes for type, 8 bytes for position
	for (int i = 0; i < buildingCount; ++i)
	{
		Building* thisBuilding = (*buildings)[i];
		short buildingType = thisBuilding->getType();
		int buildingX, buildingY;
		thisBuilding->getPosition(&buildingX, &buildingY);

		// write these
		openFile->write((char*)&buildingType, 2); // building ID
		openFile->write((char*)&buildingX, 4); // x position
		openFile->write((char*)&buildingY, 4); // y position
	}
}

// below functions are just to demonstrate reading/writing from random points

bool SaveManager::saveBuildings()
{
	std::fstream file;
	file.open(SAVEFILE_NAME, std::ios::in | std::ios::out | std::ios::binary);

	if (!file.is_open())
	{
		printf("Couldn't open save file when saving buildings!!\n");
		return false;
	}

	// grab world size so we know how far into the file to read
	int worldWidth = WORLD_WIDTH;
	int worldHeight = WORLD_HEIGHT;

	BuildingList* buildings = m_game->getBuildingManager()->getBuildings();
	int buildingCount = buildings->getCount();

	// jump to where the building count is
	file.seekp(m_headerSize - 4, std::ios::beg);
	// and save it
	file.write((char*)&buildingCount, 4);

	// then jump past the tiles
	file.seekp(m_headerSize + (worldWidth * worldHeight), std::ios::beg);

	// write em
	writeBuildings(&file);

	return true;
}

bool SaveManager::saveTiles()
{
	std::fstream file;
	file.open(SAVEFILE_NAME, std::ios::in | std::ios::out | std::ios::binary);

	if (!file.is_open())
	{
		printf("Couldn't open save file when saving tiles!!\n");
		return false;
	}

	// get the world size for writing
	int worldWidth = WORLD_WIDTH;
	int worldHeight = WORLD_HEIGHT;
	// jump to the world size, just after the first integer
	file.seekp(4, std::ios::beg);
	// write the size
	file.write((char*)&worldWidth, 4);
	file.write((char*)&worldHeight, 4);

	// jump forward 4 bytes past the building count
	file.seekp(4, std::ios::cur);

	// now we can start writing the tiles!
	writeTiles(&file, worldWidth, worldHeight);

	return true;
}

bool SaveManager::loadBuildings()
{
	std::fstream file;
	file.open(SAVEFILE_NAME, std::ios::in | std::ios::binary);

	if (!file.is_open())
	{
		printf("Couldn't open save file when loading buildings!!\n");
		return false;
	}

	// grab the number of buildings
	int buildingCount = getBuildingCount(&file);
	// grab the world size so we know how far into the file to read
	int worldWidth, worldHeight;
	getWorldSize(&worldWidth, &worldHeight, &file);

	// seek past the header and the whole world
	int buildingStart = m_headerSize + (worldWidth * worldHeight);
	file.seekg(buildingStart);

	printf("Loading %d buildings!\n", buildingCount);

	readBuildings(&file, buildingCount);

	return true;
}

bool SaveManager::loadTiles()
{
	std::fstream file;
	file.open(SAVEFILE_NAME, std::ios::in | std::ios::binary);

	if (!file.is_open())
	{
		printf("Couldn't open save file when loading tiles!!\n");
		return false;
	}

	// grab the world size
	int worldWidth, worldHeight;
	getWorldSize(&worldWidth, &worldHeight, &file);

	// skip to the start of tile data (just after header)
	file.seekg(m_headerSize, std::ios::beg);

	// grab tiles!
	readTiles(&file, worldWidth, worldHeight);

	return true;
}
