#pragma once

#include "building.h"

class Road : public Building
{
public:
	Road(Game* game, int x, int y);

	virtual void draw(aie::Renderer2D* renderer) override;

	//------------------------------------------------------------------------
	// Gets the index of the building as if it was in a 1D array instead of
	// a 2D array
	// Just returns: xIndex + (yIndex * rowWidth)
	//
	// Return: 
	//			Vector2 containing the world-based position of the building
	//------------------------------------------------------------------------
	int getOneDimensionalIndex();
};