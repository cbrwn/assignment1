#pragma once

#include "building.h"

class Shop : public Building
{
public:
	Shop(Game* game, int x, int y);

	virtual void draw(aie::Renderer2D* renderer) override;
};