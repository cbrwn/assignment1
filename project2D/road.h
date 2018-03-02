#pragma once

#include "building.h"

class Road : public Building
{
public:
	Road(Game* game, int x, int y);

	virtual void draw(aie::Renderer2D* renderer) override;
	virtual void update() override;

	char cfield;
};