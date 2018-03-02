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

	updateRoadTextures();
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

	updateRoadTextures();
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

void RoadManager::updateRoadTextures()
{


	// update road textures based on neighbouring roads
	for (auto r : m_roads)
	{
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

		r->cfield = connectField;

		// straight up/down
		// field is either 0b1000, 0b0100 or 0b1100
		if (connectField % 4 == 0)
		{
			r->setTexture(m_game->getImageManager()->getTexture("buildings/road_left"));
			continue;
		}

		// straight left/right
		// field is either 0b0001, 0b0010, 0b0011 or 0b0000
		if (connectField <= 0b0011)
		{
			r->setTexture(m_game->getImageManager()->getTexture("buildings/road_right"));
			continue;
		}

		// check for intersection
		// intersection should be used when all 4 bits are on
		//   (this tile is surrounded by roads)
		if (connectField == 0b1111)
		{
			r->setTexture(m_game->getImageManager()->getTexture("buildings/road_intersection"));
			continue;
		}

		// if nothing else was done, we can use the result of the field in the filename
		char texName[64];
		sprintf_s(texName, 64, "buildings/road_turn%d", connectField);
		r->setTexture(m_game->getImageManager()->getTexture(texName));
	}
}
