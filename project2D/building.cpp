#include "building.h"

#include "game.h"
#include "tile.h"
#include "tilemanager.h"
#include "imagemanager.h"
#include "buildingmanager.h"

char* Building::buildingNames[BUILDINGTYPE_COUNT] = {
	"None",
	"Power Plant",
	"Power Pole",
	"Road",
	"House",
	"Shop",
	"Factory"
};

Building::Building(Game* game, int x, int y)
	: m_game(game), m_posX(x), m_posY(y)
{
	// default values
	m_texture = m_game->getImageManager()->getTexture("buildings/house");
	m_type = BUILDINGTYPE_NONE;
	m_sizeX = 1;
	m_sizeY = 1;
	m_buildStyle = BUILDSTYLE_SINGLE;
	m_hasPower = false;
	m_powerSpreadRange = 2;
	m_powerSearchRange = 1;
	m_producesPower = false;
	m_shakesCamera = true;
	m_altitude = 10000.0f;
	m_fallSpeed = 20000.0f;

	m_tileAffectRange = 0;

	// default price is too expensive to buy
	m_price = INT_MAX;

	// grab the world position
	m_worldPos =
		m_game->getTileManager()->getTileWorldPosition(m_posX + 1, m_posY);
}

void Building::update(float delta)
{
	// fall from the sky!
	if (m_altitude > 0)
	{
		m_altitude -= m_fallSpeed * delta;

		// landed!
		if (m_altitude <= 0)
		{
			m_altitude = 0;

			if (m_shakesCamera)
				m_game->doScreenShake((float)m_sizeX * m_sizeY);

			// smokey particles on the front-left side
			for (int i = m_posX + 1; i > m_posX - m_sizeX; --i)
			{
				const int y = m_posY + 1;
				Vector2 v =
					m_game->getTileManager()->getTileWorldPosition(i, y);
				m_game->spawnSmokeParticle(v);
			}
			// and front-right side
			for (int i = m_posY + 1; i >= m_posY - m_sizeY; --i)
			{
				const int x = m_posX + 1;
				Vector2 v =
					m_game->getTileManager()->getTileWorldPosition(x, i);
				m_game->spawnSmokeParticle(v);
			}
		}
	}
}

void Building::created()
{
	affectOrUnaffectTiles(true);
}

void Building::destroyed()
{
	affectOrUnaffectTiles(false);
}

// this exists so we don't have to copy/paste the tile selecting code
//   in both created() and destroyed()
void Building::affectOrUnaffectTiles(bool affect)
{
	// grab versions of the size minus one because buildings with a size of 1
	//   don't extend beyond their position
	int aSizeX = m_sizeX - 1;
	int aSizeY = m_sizeY - 1;

	int minX = m_posX - aSizeX - m_tileAffectRange;
	int minY = m_posY - aSizeY - m_tileAffectRange;
	int maxX = m_posX + m_tileAffectRange;
	int maxY = m_posY + m_tileAffectRange;

	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			if (!m_game->getTileManager()->isIndexInBounds(x, y))
				continue;
			Tile* t = m_game->getTileManager()->getTile(x, y);
			if (affect)
				this->affectTile(t);
			else
				this->unaffectTile(t);
		}
	}
}

// do nothing by default
void Building::affectTile(Tile* t) {}
void Building::unaffectTile(Tile* t) {}

void Building::drawEyeball(aie::Renderer2D* renderer, Vector2& pos,
	const float rad) const
{
	// get mouse position
	Vector2 mousePos = m_game->getMouseWorldPosition();

	// get angle to mouse
	mousePos -= pos;
	float angle = mousePos.angle();

	float eyeSize = rad;
	const float pupilSize = eyeSize / 2.0f;

	// white of eye
	renderer->setRenderColour(1, 1, 1);
	renderer->drawCircle(pos.getX(), pos.getY(), eyeSize);

	// pupil!
	// base its distance from the center on the size
	float dist = eyeSize - pupilSize;
	// and get its x and y offsets from the angle
	float xOff = cosf(angle) * dist;
	float yOff = sinf(angle) * dist;

	renderer->setRenderColour(0, 0, 0);
	renderer->drawCircle(pos.getX() + xOff, pos.getY() + yOff, pupilSize);
}

void Building::setPosition(int x, int y)
{
	m_posX = x;
	m_posY = y;

	// update world positions
	m_worldPos =
		m_game->getTileManager()->getTileWorldPosition(m_posX + 1, m_posY);
}

void Building::getCenter(int* x, int* y) const
{
	*x = m_posX - (m_sizeX - 1) / 2;
	*y = m_posY - (m_sizeY - 1) / 2;
}

// checks if this building should have power and also spreads it if needed
bool Building::updatePower()
{
	TileManager* tMan = m_game->getTileManager();

	// check if a tile of ours has power
	int maxX = m_posX + m_powerSearchRange;
	int maxY = m_posY + m_powerSearchRange;
	int minX = m_posX - (m_sizeX - 1) - m_powerSearchRange;
	int minY = m_posY - (m_sizeY - 1) - m_powerSearchRange;

	m_hasPower = m_producesPower;
	if (!m_hasPower)
	{
		// if we have no power, search for it!
		for (int y = minY; y <= maxY; ++y)
		{
			for (int x = minX; x <= maxX; ++x)
			{
				if (x < 0 || y < 0 || x >= WORLD_WIDTH || y >= WORLD_HEIGHT)
					continue;

				Tile* thisTile = tMan->getTile(x, y);
				if (!thisTile)
					continue;
				if (thisTile->hasPower())
				{
					m_hasPower = true;
					break;
				}
			}
			if (m_hasPower)
				break;
		}
	}

	// don't continue with this if this building has no power
	if (!m_hasPower)
		return false;

	// make values larger for spreading power
	maxX = m_posX + m_powerSpreadRange;
	maxY = m_posY + m_powerSpreadRange;
	minX = m_posX - (m_sizeX - 1) - m_powerSpreadRange;
	minY = m_posY - (m_sizeY - 1) - m_powerSpreadRange;

	// we want to return true if this changed the state of the power
	bool gavePower = false;

	// pass power on to the next few tiles
	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			if (tMan->isIndexInBounds(x, y))
				continue;

			Tile* thisTile = tMan->getTile(x, y);
			if (!thisTile)
				continue;
			if (!thisTile->hasPower())
			{
				gavePower = true;
				thisTile->setPower(true);
			}
		}
	}

	return gavePower;
}
