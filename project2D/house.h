#pragma once

#include "building.h"

class House : public Building
{
public:
	House(Game* game, int x, int y);

	virtual void draw(aie::Renderer2D* renderer) override;
};