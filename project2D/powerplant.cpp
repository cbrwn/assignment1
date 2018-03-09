#include "buildingmanager.h"
#include "powerplant.h"
#include "powerpole.h"
#include "tile.h"
#include "game.h"

PowerPlant::PowerPlant(Game* game, int x, int y)
	: Building(game, x, y)
{
	m_type = BuildingType::BUILDINGTYPE_POWERPLANT;
	m_sizeX = 4;
	m_sizeY = 4;
	m_powerSpreadRange = 3;
	m_producesPower = true;

	m_texture = game->getImageManager()->getTexture("buildings/powerplant");
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
	drawEyeball(renderer, Vector2(m_worldPos.getX() + 84.0f, 
		m_worldPos.getY() + 128.0f + m_altitude));
	drawEyeball(renderer, Vector2(m_worldPos.getX() + 194.0f, 
		m_worldPos.getY() + 184.0f + m_altitude));
}