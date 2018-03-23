#include <iostream>

#include "darray.h"
#include "game.h"
#include "imagemanager.h"
#include "roadmanager.h"
#include "road.h"

RoadManager::RoadManager(Game* game)
{
	m_game = game;
	m_roads = new RoadList;
}

RoadManager::~RoadManager()
{
	delete m_roads;
}

void RoadManager::addRoad(Building* newRoad, bool sort)
{
	if (newRoad->getType() != BUILDINGTYPE_ROAD)
		return;

	m_roads->add((Road*)newRoad);

	if (!sort)
		return;

	// update textures if there's one adjacent to this new one
	bool isAdjacent = false;
	int newPosX, newPosY;
	newRoad->getPosition(&newPosX, &newPosY);
	for (int i = -1; i <= 1; ++i)
	{
		Road* adjHorz = getRoadAtPosition(newPosX + i, newPosY);
		Road* adjVert = getRoadAtPosition(newPosX, newPosY + i);
		if (adjHorz || adjVert)
		{
			isAdjacent = true;
			break;
		}
	}
	if (isAdjacent)
		updateRoadTextures();
}

void RoadManager::updateRoads()
{
	// sort
	int roadCount = m_roads->getCount();
	quickSortRoads(0, roadCount - 1);
	updateRoadTextures();
}

void RoadManager::removeRoad(Building* road)
{
	// remove our copy of this road pointer
	m_roads->remove((Road*)road);

	updateRoadTextures();
}

void RoadManager::clearRoads()
{
	m_roads->clear();
}

// binary search for road at the given position
Road* RoadManager::getRoadAtPosition(int x, int y)
{
	// make sure index is within bounds of world
	if (x >= WORLD_WIDTH || y >= WORLD_HEIGHT
		|| x < 0 || y < 0)
		return false;

	// no roads to search for!
	if (m_roads->getCount() < 1)
		return false;

	// roads are sorted by (y*width)+x, like how they would be laid out in
	//   a 1d array, so we get that as our target
	int targetIndex = (y*WORLD_WIDTH) + x;

	// binary search time
	int min = -1;
	int max = m_roads->getCount();
	int mid = max / 2;

	// grab the index of the mid road
	int midX, midY;
	(*m_roads)[mid]->getPosition(&midX, &midY);
	int midIndex = (midY*WORLD_WIDTH) + midX;
	while (midIndex != targetIndex)
	{
		// adjust the search parameters based on the index of the road
		if (midIndex < targetIndex)
			min = mid;
		else if (midIndex > targetIndex)
			max = mid;
		mid = (min + max) / 2;

		// check if there's no more to search - road doesn't exist at index
		if (max - min <= 1)
			break;

		// update the road index
		Road* midRoad = (*m_roads)[mid];
		midRoad->getPosition(&midX, &midY);
		midIndex = (midY*WORLD_WIDTH) + midX;
	}

	// road was found! return it
	if (midIndex == targetIndex)
		return (*m_roads)[mid];

	// wasn't found
	return nullptr;
}

Road* RoadManager::getClosestRoad(int x, int y, int* distOut)
{
	Road* closest = nullptr;
	int closestDist = INT_MAX;

	for (int i = 0; i < m_roads->getCount(); i++)
	{
		Road* r = (*m_roads)[i];
		int rx, ry;
		r->getPosition(&rx, &ry);
		int xDist = (int)fabsf((float)x - rx);
		int yDist = (int)fabsf((float)y - ry);
		int totalDist = xDist + yDist;
		if (totalDist < closestDist)
		{
			closest = r;
			closestDist = totalDist;
		}
	}

	*distOut = closestDist;
	return closest;
}

void RoadManager::updateRoadTextures()
{
	// update road textures based on neighbouring roads
	for (int i = 0; i < m_roads->getCount(); i++)
	{
		Road* r = (*m_roads)[i];
		// grab the road positions
		int ix, iy;
		r->getPosition(&ix, &iy);

		// use a bitfield to hold which sides are connected
		// 0bUDLR
		// U = y-1, D = y+1, L = x-1, R = x+1
		char connectField = 0;
		if (getRoadAtPosition(ix, iy - 1)) // road is above
			connectField |= 0b1000;
		if (getRoadAtPosition(ix, iy + 1)) // road is below
			connectField |= 0b0100;
		if (getRoadAtPosition(ix - 1, iy)) // road is to the left
			connectField |= 0b0010;
		if (getRoadAtPosition(ix + 1, iy)) // road is to the right
			connectField |= 0b0001;

		// straight up/down
		// field is either 0b1000, 0b0100 or 0b1100
		if (connectField % 4 == 0)
		{
			r->setTexture(m_game->getImageManager()->getTexture(
				"buildings/road_left"));
			continue;
		}

		// straight left/right
		// field is either 0b0001, 0b0010, 0b0011 or 0b0000
		if (connectField <= 0b0011)
		{
			r->setTexture(m_game->getImageManager()->getTexture(
				"buildings/road_right"));
			continue;
		}

		// check for intersection
		// intersection should be used when all 4 bits are on
		//   (this tile is surrounded by roads)
		if (connectField == 0b1111)
		{
			r->setTexture(m_game->getImageManager()->getTexture(
				"buildings/road_intersection"));
			continue;
		}

		// if nothing else was done, we can use the result of the field in the 
		// filename
		char texName[64];
		sprintf_s(texName, 64, "buildings/road_turn%d", connectField);
		r->setTexture(m_game->getImageManager()->getTexture(texName));
	}
}

void RoadManager::quickSortRoads(int min, int max)
{
	if (min < max)
	{
		int p = partitionRoads(min, max);
		// now sort both sides of the partition index
		quickSortRoads(min, p - 1);
		quickSortRoads(p + 1, max);
	}
}

// based on pseudocode from the Lomuto partition scheme:
// https://en.wikipedia.org/wiki/Quicksort#Lomuto_partition_scheme
int RoadManager::partitionRoads(int min, int max)
{
	// use last road as the pivot
	Road* pivot = (*m_roads)[max];
	int pivIndex = pivot->getOneDimensionalIndex();

	int i = min - 1;
	for (int j = min; j < max; ++j)
	{
		int thisIndex = (*m_roads)[j]->getOneDimensionalIndex();
		if (thisIndex < pivIndex)
		{
			i++;
			// swap
			Road* iRoad = (*m_roads)[i];
			(*m_roads)[i] = (*m_roads)[j];
			(*m_roads)[j] = iRoad;
		}
	}
	// put the pivot in its place
	(*m_roads)[max] = (*m_roads)[i + 1];
	(*m_roads)[i + 1] = pivot;
	return i + 1;
}
