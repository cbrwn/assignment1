////////////////////////////////////////////
// Power Pole
// Carries electricity from Power Plants to
//   everything that uses it
////////////////////////////////////////////

#include "powerpole.h"
#include "game.h"

PowerPole::PowerPole(Game* game, int x, int y)
	: Building(game, x, y)
{
	m_type = BuildingType::POWERPOLE;
	m_buildStyle = BuildStyle::LINE;
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

	// keep origin at the bottom-middle of the sprite
	// (where it would be touching the ground)
	const float xOrigin = 0.5f;
	const float yOrigin = 0.0f;
	renderer->drawSprite(m_texture, wx, wy, 0, 0, 0, 0, xOrigin, yOrigin);

	if (m_hasPower)
	{
		renderer->setRenderColour(0, 1, 0);
		renderer->drawCircle(wx, wy, 5);
	}
}

void PowerPole::update()
{
	Building::update();
}

// returns whether or not building b is directly next to this pole
bool PowerPole::isTouchingBuilding(Building* b)
{
	// grab the bounds of this building
	int right, bottom;
	b->getPosition(&right, &bottom);
	int w, h;
	b->getSize(&w, &h);
	int left = right - (w - 1);
	int top = bottom - (h - 1);

	// get distances from each side
	int distLeft = abs(left - m_posX);
	int distRight = abs(right - m_posX);
	int distTop = abs(top - m_posY);
	int distBottom = abs(bottom - m_posY);

	// get the shortest distance on each axis
	int closestSideX = distLeft;
	if (distRight < closestSideX)
		closestSideX = distRight;
	int closestSideY = distTop;
	if (distBottom < closestSideY)
		closestSideY = distBottom;

	// this tile is touching if the sum of the shortest distances isn't
	// larger than 1
	return ((closestSideX + closestSideY) <= 1);
}
