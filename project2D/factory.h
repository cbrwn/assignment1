#pragma once

#include "building.h"

#define FACTORY_POLLUTION_AMT 10

class Factory : public Building
{
public:
	Factory(Game* game, int x, int y);

	void update(float delta) override;
	virtual void draw(aie::Renderer2D* renderer) override;

	void affectTile(Tile* t) override;
	void unaffectTile(Tile* t) override;
private:
	float m_smokePuffTime;
	aie::Texture* m_pollutionTexture;
};