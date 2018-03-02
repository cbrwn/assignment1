#pragma once

#include <vector>
#include "building.h"

class PowerPole : public Building
{
public:
	PowerPole(Game* game, int x, int y);

	virtual void draw(aie::Renderer2D* renderer) override;
};