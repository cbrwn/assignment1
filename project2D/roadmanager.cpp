#include <iostream>

#include "game.h"
#include "roadmanager.h"
#include "road.h"

RoadManager::RoadManager(Game* game)
{
	m_game = game;
}

void RoadManager::addRoad(Building* newRoad)
{
	if (newRoad->getType() != BUILDINGTYPE_ROAD)
		return;

	m_roads.push_back((Road*)newRoad);

	// sort
	// todo: use a better sort algorithm
	int roadCount = (int)m_roads.size();
	bool changed = true;
	while (changed)
	{
		changed = false;
		for (int i = 0; i < roadCount - 1; ++i)
		{
			for (int j = roadCount - 1; j >= i + 1; --j)
			{
				// get first road's index
				int ax, ay;
				m_roads[i]->getPosition(&ax, &ay);
				int aIndex = (ay * WORLD_WIDTH) + ax;

				// and second road's index
				int bx, by;
				m_roads[i + 1]->getPosition(&bx, &by);
				int bIndex = (by * WORLD_WIDTH) + bx;

				if (bIndex < aIndex)
				{
					// swap!
					Road* aRoad = m_roads[i];
					m_roads[i] = m_roads[i + 1];
					m_roads[i + 1] = aRoad;
					changed = true;
				}
			}
		}
	}
}

void RoadManager::removeRoad(Building* road)
{
	// remove our copy of this road pointer
	for (RoadList::iterator it = m_roads.begin(); it != m_roads.end(); ++it)
	{
		if (road == *it)
		{
			m_roads.erase(it);
			break;
		}
	}
}

// binary search for road at the given position
Road* RoadManager::getRoadAtPosition(int x, int y)
{
	// make sure index is within bounds of world
	if (x >= WORLD_WIDTH || y >= WORLD_HEIGHT
		|| x < 0 || y < 0)
		return false;

	// no roads to search for!
	if ((int)m_roads.size() < 1)
		return false;

	// roads are sorted by (y*width)+x, like how they would be laid out in
	//   a 1d array, so we get that as our target
	int targetIndex = (y*WORLD_WIDTH) + x;

	// binary search time
	int min = 0;
	int max = (int)m_roads.size() - 1;
	int mid = max / 2;

	// grab the index of the mid road
	int midX, midY;
	m_roads[mid]->getPosition(&midX, &midY);
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
		Road* midRoad = m_roads[mid];
		midRoad->getPosition(&midX, &midY);
		midIndex = (midY*WORLD_WIDTH) + midX;
	}

	// road was found! return it
	if (midIndex == targetIndex)
		return m_roads[mid];

	// wasn't found
	return nullptr;
}
