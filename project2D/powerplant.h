#pragma once

#include "building.h"

class PowerPlant : public Building
{
public:
	PowerPlant(Game* game, int x, int y);

	void update(float delta) override;
	void draw(aie::Renderer2D* renderer) override;

	void created() override;
private:
	aie::Texture* m_openMouth;
	aie::Texture* m_closedMouth;

	bool m_drawFace;
	bool m_blinking;
	bool m_mouthOpen;
	float m_blinkTimer;
};