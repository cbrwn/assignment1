#pragma once

// Forward declares
template <class T>
class DArray;

class Building;
class Game;
class Road;

// typedef for shorter typing
typedef DArray<Road*> RoadList;

class RoadManager
{
public:
	//------------------------------------------------------------------------
	// Param:
	//			game:		pointer to our Game so we can access everything
	//------------------------------------------------------------------------
	explicit RoadManager(Game* game);
	~RoadManager();

	// we don't expect RoadManager to be copied/moved
	RoadManager(const RoadManager& rm) = default;
	RoadManager(RoadManager&& rm) = default;
	RoadManager& operator=(RoadManager& rm) = default;
	RoadManager& operator=(RoadManager&& rm) = default;

	//------------------------------------------------------------------------
	// Adds a road to the list
	// Should be called whenever a road is made so we can keep track of it
	//
	// Param: 
	//			newRoad: pointer to the road we're adding
	//			sort:	 whether or not to sort the list of roads
	//------------------------------------------------------------------------
	void addRoad(Building* newRoad, bool sort = true);
	//------------------------------------------------------------------------
	// Removes a road from the list
	// Should be called whenever a road is destroyed
	//
	// Param: 
	//			road: road to remove from the list
	//------------------------------------------------------------------------
	void removeRoad(Building* road);
	//------------------------------------------------------------------------
	// Clears the list of roads
	//------------------------------------------------------------------------
	void clearRoads();

	//------------------------------------------------------------------------
	// Sorts the list of roads and updates their textures
	// Should be called whenever a road is added/removed
	//------------------------------------------------------------------------
	void updateRoads();

	//------------------------------------------------------------------------
	// Gets the road at at a position
	// Returns nullptr if there is no road at the position
	//
	// Param: 
	//			x: tile-based x position to look for
	//			y: tile-based y position to look for
	// Return: 
	//			pointer to the road found at the position
	//------------------------------------------------------------------------
	Road* getRoadAtPosition(int x, int y);
	//------------------------------------------------------------------------
	// Gets the road closest to a position and outputs the distance to it
	//
	// Param: 
	//			x:		 tile-based x position to compare
	//			y:		 tile-based y position to compare
	//			distOut: pointer to where the distance should output
	// Return: 
	//			pointer to the road closest to the position
	//------------------------------------------------------------------------
	Road* getClosestRoad(int x, int y, int* distOut);
private:
	Game* m_game;

	RoadList* m_roads;

	// function which changes the roads' textures based on their neighbours
	void updateRoadTextures();

	// sorting functions
	void quickSortRoads(int min, int max);
	int partitionRoads(int min, int max);
};