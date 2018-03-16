#include "buildingmanager.h"
#include "powerplant.h"
#include "powerpole.h"
#include "tile.h"
#include "game.h"
#include "imagemanager.h"
#include "random.h"

PowerPlant::PowerPlant(Game* game, int x, int y)
	: Building(game, x, y)
{
	m_type = BuildingType::BUILDINGTYPE_POWERPLANT;
	m_sizeX = 4;
	m_sizeY = 4;
	m_powerSpreadRange = 3;
	m_producesPower = true;

	m_price = 1000;

	m_texture = game->getImageManager()->getTexture("buildings/powerplant");
	
	// eyes and mouth stuff
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

	const float blinkLength = 0.1f;

	if (m_blinkTimer <= 0)
	{
		m_blinking = !m_blinking;
		if (m_blinking)
			m_blinkTimer = blinkLength;
		else
			m_blinkTimer = randBetween(1.0f, 10.0f);
	}

	Vector2 mousePos = m_game->getMouseWorldPosition();
	Vector2 thisPos = m_worldPos + Vector2(0, 256);
	float dist = mousePos.distanceToSquared((thisPos));

	m_mouthOpen = dist < 350*350;
}

void PowerPlant::draw(aie::Renderer2D* renderer)
{
	if (m_posX < 0 || m_posY < 0)
		return;

	// keep origin at the bottom-middle of the sprite
	// (where it would be touching the ground)
	const float xOrigin = 0.5f;
	const float yOrigin = 0.0f;
	renderer->drawSprite(m_texture, m_worldPos.getX(),
		m_worldPos.getY() - 4 + m_altitude, 0, 0, 0, 0, xOrigin, yOrigin);

	// eyeballs
	if (!m_blinking)
	{
		drawEyeball(renderer, Vector2(m_worldPos.getX() + 84.0f,
			m_worldPos.getY() + 148.0f + m_altitude), 48.0f);
		drawEyeball(renderer, Vector2(m_worldPos.getX() + 194.0f,
			m_worldPos.getY() + 204.0f + m_altitude), 48.0f);
	}

	// draw mouth
	aie::Texture* mtex = m_mouthOpen ? m_openMouth : m_closedMouth;
	renderer->setRenderColour(1, 1, 1);
	renderer->drawSprite(mtex, m_worldPos.getX() + 161.0f, 
		m_worldPos.getY() + 120.0f + m_altitude);
}