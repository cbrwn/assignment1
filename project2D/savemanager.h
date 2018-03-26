#pragma once

#include <fstream>

// Forward declares
class Game;

#define SAVEFILE_NAME "city.wld"

class SaveManager
{
public:
	//------------------------------------------------------------------------
	// (explicit because we don't want any implicit conversion)
	//
	// Param:
	//			game: pointer to our Game so we can access everything we need
	//------------------------------------------------------------------------
	explicit SaveManager(Game* game);

	//------------------------------------------------------------------------
	// Loads all data from the save file
	// This includes money, world size, zone placement and buildings
	//------------------------------------------------------------------------
	bool loadData();
	//------------------------------------------------------------------------
	// Saves all data to the save file
	// This includes money, world size, zone placement and buildings
	//------------------------------------------------------------------------
	bool saveData();

	//------------------------------------------------------------------------
	// Saves only the buildings and the building count to the save file
	//------------------------------------------------------------------------
	bool saveBuildings();
	//------------------------------------------------------------------------
	// Saves only the zones of the tiles and the world size to the save file
	//------------------------------------------------------------------------
	bool saveTiles();

	//------------------------------------------------------------------------
	// Loads only the buildings from the save file
	//------------------------------------------------------------------------
	bool loadBuildings();
	//------------------------------------------------------------------------
	// Loads only the tiles zones from the save file
	//------------------------------------------------------------------------
	bool loadTiles();
private:
	Game* m_game;

	// how many bytes the starting information takes up
	// consisting of money, map size and building count
	const int m_headerSize;

	// used in loadTiles and loadBuildings
	void getWorldSize(int* width, int* height, std::fstream* openFile);
	int getBuildingCount(std::fstream* openFile);

	// functions to reduce duplicate code in load/save Buildings/Tiles
	void readTiles(std::fstream* openFile, int worldWidth, int worldHeight);
	void readBuildings(std::fstream* openFile, int buildingCount);
	void writeTiles(std::fstream* openFile, int worldWidth, int worldHeight);
	void writeBuildings(std::fstream* openFile);
};