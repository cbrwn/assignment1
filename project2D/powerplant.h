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
	// textures for the face of the building
	aie::Texture* m_openMouth;
	aie::Texture* m_closedMouth;

	// whether or not the face should be drawn
	bool m_drawFace;
	// whether or not the eyes should be drawn
	bool m_blinking;
	// determines which mouth texture to draw
	bool m_mouthOpen;
	// time since the last blink
	float m_blinkTimer;
};