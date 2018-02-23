////////////////////////////////////////////
// Power Plant
// Generates power to provide the city with
//   electricity
////////////////////////////////////////////

#include "powerplant.h"
#include "game.h"

PowerPlant::PowerPlant(Game* game, int x, int y)
	: Building(game, x, y)
{
	m_type = BuildingType::POWERPLANT;
	m_sizeX = 4;
	m_sizeY = 4;
	
	m_texture = game->getImageManager()->getTexture("buildings/powerplant");
}

void PowerPlant::draw(aie::Renderer2D* renderer)
{
	if (m_posX < 0 || m_posY < 0)
		return;

	// grab the world position of our base tile
	float wx, wy;
	// x + 1 to account for the width of the tile
	m_game->getTileWorldPosition(m_posX+1, m_posY, &wx, &wy);

	// keep origin at the bottom-middle of the sprite
	// (where it would be touching the ground)
	const float xOrigin = 0.5f;
	const float yOrigin = 0.0f;
	renderer->drawSprite(m_texture, wx, wy, 0, 0, 0, 0, xOrigin, yOrigin);
}

void PowerPlant::update()
{
}
