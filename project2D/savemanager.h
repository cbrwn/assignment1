#pragma once

#include <fstream>

class Game;

#define SAVEFILE_NAME "city.wld"

class SaveManager
{
public:
	explicit SaveManager(Game* game);

	bool loadData();
	bool saveData();

	// saving and loading specific parts
	bool saveBuildings();
	bool saveTiles();

	bool loadBuildings();
	bool loadTiles();
private:
	Game* m_game;

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