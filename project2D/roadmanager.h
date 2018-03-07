#pragma once

#include <vector>

class Building;
class Game;
class Road;

// typedef vector for shorter typings
typedef std::vector<Road*> RoadList;

class RoadManager
{
public:
	RoadManager(Game* game);

	void addRoad(Building* newRoad, bool sort = true);
	void removeRoad(Building* road);

	void updateRoads();

	Road* getRoadAtPosition(int x, int y);
private:
	Game* m_game;

	RoadList m_roads;

	void updateRoadTextures();

	void quickSortRoads(int min, int max);
	int partitionRoads(int min, int max);
};