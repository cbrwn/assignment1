#include "powerplant.h"

#include "game.h"
#include "random.h"
#include "imagemanager.h"

PowerPlant::PowerPlant(Game* game, int x, int y)
	: Building(game, x, y)
{
	m_type = BUILDINGTYPE_POWERPLANT;
	m_sizeX = 4;
	m_sizeY = 4;
	m_powerSpreadRange = 3;
	m_producesPower = true;

	m_price = 1000;

	m_texture = game->getImageManager()->getTexture("buildings/powerplant");

	// eyes and mouth stuff
	m_drawFace = false;
	m_blinking = false;
	m_mouthOpen = false;
	m_blinkTimer = 0;

	m_openMouth = game->getImageManager()->getTexture("mouth_open");
	m_closedMouth = game->getImageManager()->getTexture("mouth_closed");
}

void PowerPlant::update(float delta)
{
	Building::update(delta);

	m_blinkTimer -= delta;

	// how long a blink should last in seconds
	const float blinkLength = 0.1f;

	if (m_blinkTimer <= 0)
	{
		m_blinking = !m_blinking;
		if (m_blinking)
			m_blinkTimer = blinkLength;
		else
			m_blinkTimer = randBetween(1.0f, 10.0f);
	}

	// open the mouth if the mouse is close to us

	Vector2 mousePos = m_game->getMouseWorldPosition();
	// add 256 to our Y so it's not the distance to the bottom of building
	Vector2 thisPos = m_worldPos + Vector2(0, 256);
	float dist = mousePos.distanceToSquared((thisPos));

	// square 350 because w're getting the distance squared
	m_mouthOpen = dist < 350 * 350;
}

void PowerPlant::draw(aie::Renderer2D* renderer)
{
	if (m_posX < 0 || m_posY < 0)
		return;

	// keep origin at the bottom-middle of the sprite
	// (where it would be touching the ground)
	const float xOrigin = 0.5f;
	const float yOrigin = 0.0f;

	// move the sprite up slightly so it lines up with the ground
	const float yOffset = -4;

	renderer->drawSprite(m_texture, m_worldPos.getX(),
		m_worldPos.getY() - yOffset + m_altitude, 0, 0, 0, 0, xOrigin, yOrigin);

	if (!m_drawFace)
		return;

	// eyeballs
	if (!m_blinking)
	{
		const Vector2 leftEyeOffset(84.0f, 148.0f);
		const Vector2 rightEyeOffset(194.0f, 204.0);
		const float eyeSize = 48.0f;

		drawEyeball(renderer, Vector2(m_worldPos.getX() + leftEyeOffset.getX(),
			m_worldPos.getY() + leftEyeOffset.getY() + m_altitude), eyeSize);
		drawEyeball(renderer, Vector2(m_worldPos.getX() + rightEyeOffset.getX(),
			m_worldPos.getY() + rightEyeOffset.getY() + m_altitude), eyeSize);
	}

	// draw mouth
	const Vector2 mouthOffset(161.0f, 120.0f);

	aie::Texture* mtex = m_mouthOpen ? m_openMouth : m_closedMouth;
	renderer->setRenderColour(1, 1, 1);
	renderer->drawSprite(mtex, m_worldPos.getX() + mouthOffset.getX(),
		m_worldPos.getY() + mouthOffset.getY() + m_altitude);
}

void PowerPlant::created()
{
	// show the face only when placed so it doesn't show up on the preview
	m_drawFace = true;
}
