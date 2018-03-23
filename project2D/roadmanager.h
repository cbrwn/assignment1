#pragma once

template <class T>
class DArray;

class Building;
class Game;
class Road;

// typedef vector for shorter typings
typedef DArray<Road*> RoadList;

class RoadManager
{
public:
	explicit RoadManager(Game* game);
	~RoadManager();

	// we don't expect Game to be copied/moved
	RoadManager(const RoadManager& rm) = default;
	RoadManager(RoadManager&& rm) = default;
	RoadManager& operator=(RoadManager& rm) = default;
	RoadManager& operator=(RoadManager&& rm) = default;

	void addRoad(Building* newRoad, bool sort = true);
	void removeRoad(Building* road);
	void clearRoads();

	void updateRoads();

	Road* getRoadAtPosition(int x, int y);
	Road* getClosestRoad(int x, int y, int* distOut);
private:
	Game* m_game;

	RoadList* m_roads;

	void updateRoadTextures();

	void quickSortRoads(int min, int max);
	int partitionRoads(int min, int max);
};