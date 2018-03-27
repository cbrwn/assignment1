#pragma once

#include "building.h"

class PowerPole : public Building
{
public:
	PowerPole(Game* game, int x, int y);

	void draw(aie::Renderer2D* renderer) override;
};