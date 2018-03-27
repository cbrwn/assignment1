#pragma once

#include "building.h"

// how much pollution a factory gives to nearby tiles
#define FACTORY_POLLUTION_AMT 10

class Factory : public Building
{
public:
	Factory(Game* game, int x, int y);

	void update(float delta) override;
	void draw(aie::Renderer2D* renderer) override;

	void affectTile(Tile* t) override;
	void unaffectTile(Tile* t) override;
private:
	// how long it has been since producing a puff to indicate pollution
	float m_smokePuffTime;
};