#pragma once

#include "building.h"

class PowerPlant : public Building
{
public:
	PowerPlant(Game* game, int x, int y);

	virtual void draw(aie::Renderer2D* renderer) override;
private:
};