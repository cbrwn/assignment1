#pragma once

#include "building.h"

class Factory : public Building
{
public:
	Factory(Game* game, int x, int y);

	void update(float delta) override;
	virtual void draw(aie::Renderer2D* renderer) override;
private:
	float m_smokePuffTime;
	aie::Texture* m_pollutionTexture;
};