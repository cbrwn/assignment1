#pragma once

#include "building.h"

class Factory : public Building
{
public:
	Factory(Game* game, int x, int y);

	virtual void draw(aie::Renderer2D* renderer) override;
};