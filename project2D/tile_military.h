#pragma once

#include "tile.h"

class TileMilitary : public Tile
{
public:
	TileMilitary(Game* game, bool right);

	virtual void  update();
	virtual void  draw(aie::Renderer2D* renderer, float dx, float dy);
	virtual char* getMouseoverText() override;

	int  getSoldiers() { return m_soldiers; }
	void addSoldiers(int amt);
private:
	int m_soldiers, m_maxSoldiers;

	aie::Texture* m_middleLayer;
	aie::Texture* m_roofTexture;
};