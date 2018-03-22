#include "factory.h"
#include "game.h"
#include "imagemanager.h"
#include "random.h"
#include "tile.h"

Factory::Factory(Game* game, int x, int y)
	: Building(game, x, y)
{
	m_type = BuildingType::BUILDINGTYPE_FACTORY;
	m_buildStyle = BuildStyle::BUILDSTYLE_FORBIDDEN;
	m_sizeX = 1;
	m_sizeY = 1;
	m_powerSearchRange = 1;
	m_powerSpreadRange = 0;
	m_shakesCamera = false;

	m_tileAffectRange = 7;

	m_texture = m_game->getImageManager()->getTexture("buildings/factory");
	m_pollutionTexture = m_game->getImageManager()->getTexture("pollution");
	m_smokePuffTime = randBetween(3.0f, 30.0f);
}

void Factory::update(float delta)
{
	Building::update(delta);

	m_smokePuffTime -= delta;
	if (m_smokePuffTime <= 0.0f)
	{
		Vector2 pos = m_worldPos;
		pos += Vector2(TILE_WIDTH/2.0f - 32.0f, TILE_HEIGHT);
		m_game->spawnPollutionParticle(pos);
		m_smokePuffTime = randBetween(3.0f, 12.0f);
	}
}

void Factory::draw(aie::Renderer2D* renderer)
{
	if (m_posX < 0 || m_posY < 0)
		return;

	const float xOrigin = 0.5f;
	const float yOrigin = 0.0f;
	renderer->drawSprite(m_texture, m_worldPos.getX(),
		m_worldPos.getY() + m_altitude - 3, 0, 0, 0, 0, xOrigin, yOrigin);
}

void Factory::affectTile(Tile* t)
{
	t->addPollution(FACTORY_POLLUTION_AMT);
}

void Factory::unaffectTile(Tile* t)
{
	t->addPollution(-FACTORY_POLLUTION_AMT);
}
