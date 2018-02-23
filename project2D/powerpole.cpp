#include "powerpole.h"
#include "game.h"

PowerPole::PowerPole(Game* game, int x, int y)
	: Building(game, x, y)
{
	m_type = BuildingType::POWERPOLE;
	m_sizeX = 1;
	m_sizeY = 1;

	m_texture = m_game->getImageManager()->getTexture("buildings/powerpole");
}

void PowerPole::draw(aie::Renderer2D* renderer)
{
	if (m_posX < 0 || m_posY < 0)
		return;
	// grab the world position of our base tile
	float wx, wy;
	m_game->getTileWorldPosition(m_posX + 1, m_posY, &wx, &wy);

	// sprite origins come from the pixel coordinate divided by the total height
	const float xOrigin = 0.5f;
	const float yOrigin = 0.0f;
	renderer->drawSprite(m_texture, wx, wy, 0, 0, 0, 0, xOrigin, yOrigin);
}

void PowerPole::update()
{
}
