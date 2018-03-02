////////////////////////////////////////////
// BuildingManager
// This class is responsible for all things
//   to do with buildings, such as creating
//   and drawing them
////////////////////////////////////////////

#include <iostream>
#include <Input.h>

#include "buildingmanager.h"
#include "roadmanager.h"
#include "game.h"

#include "building.h"
#include "powerplant.h"
#include "powerpole.h"
#include "road.h"

BuildingManager::BuildingManager(Game* game, BuildingList* buildings)
	: m_game(game), m_buildings(buildings)
{
	m_selectedBuilding = 0;
	m_updateTimer = 0;
	m_ghostBuilding = nullptr;
}

BuildingManager::~BuildingManager()
{
	if (m_ghostBuilding)
		delete m_ghostBuilding;
}

void BuildingManager::buildingMode()
{
	// double check to make sure we're in building mode
	if (m_game->getPlaceMode() != PlaceMode::PLACEMODE_BUILDING)
		return;

	aie::Input* input = aie::Input::getInstance();

	// B to toggle building mode
	if (input->wasKeyPressed(aie::INPUT_KEY_B))
	{
		m_game->setPlaceMode(PlaceMode::PLACEMODE_NONE);
		return;
	}

	// controlling the selected building with keyboard
	if (input->wasKeyPressed(aie::INPUT_KEY_PERIOD))
		m_selectedBuilding++;
	if (input->wasKeyPressed(aie::INPUT_KEY_COMMA))
		m_selectedBuilding--;

	// wrap around
	if (m_selectedBuilding >= (int)BUILDINGTYPE_COUNT)
		m_selectedBuilding = 0;
	if (m_selectedBuilding < 0)
		m_selectedBuilding = (int)BUILDINGTYPE_COUNT - 1;

	// grab the mouse position
	float mx, my;
	m_game->getMouseWorldPosition(&mx, &my);

	// get the index of the tile we're mousing over
	int tileX, tileY;
	m_game->getTileAtPosition(mx, my, &tileX, &tileY);

	// update the ghost building to show which building we're choosing
	if (m_ghostBuilding == nullptr
		|| (int)m_ghostBuilding->getType() != m_selectedBuilding)
	{
		if (m_ghostBuilding)
			delete m_ghostBuilding;
		m_ghostBuilding = makeBuilding((BuildingType)m_selectedBuilding,
			tileX, tileY, true);
	}

	// make sure the ghost building is where our mouse is
	if (m_ghostBuilding)
		m_ghostBuilding->setPosition(tileX, tileY);

	// demolish buildings
	if (m_selectedBuilding == BUILDINGTYPE_NONE
		&& input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_LEFT))
	{
		Building* underMouse = getBuildingAtIndex(tileX, tileY);
		if (underMouse)
			removeBuilding(underMouse);
	}

	// we can stop here if placing things isn't allowed
	if (!(canPlaceBuilding() && m_game->isMouseInGame()))
		return;

	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT)
		&& m_ghostBuilding->getBuildStyle() == BuildStyle::BUILDSTYLE_SINGLE)
	{
		// place building!
		addBuilding(makeBuilding((BuildingType)m_selectedBuilding,
			tileX, tileY));
		// cancel building mode if shift isn't held
		// so player can place multiple buildings easily by holding shift
		if (!input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT))
			m_game->setPlaceMode(PlaceMode::PLACEMODE_NONE);
	}

	// line style lets you click and drag
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_LEFT)
		&& m_ghostBuilding->getBuildStyle() == BuildStyle::BUILDSTYLE_LINE)
	{
		addBuilding(makeBuilding((BuildingType)m_selectedBuilding,
			tileX, tileY));
	}
}

// draws everything while in building mode
// basically just the translucent ghost building to show the player
//   what they're building
void BuildingManager::drawPlacement(aie::Renderer2D* renderer)
{
	if (m_ghostBuilding)
	{
		const float alpha = 0.4f;
		// tint green if we can place
		if (canPlaceBuilding())
			renderer->setRenderColour(0, 1, 0, alpha);
		else // and red if not
			renderer->setRenderColour(1, 0, 0, alpha);
		m_ghostBuilding->draw(renderer);
	}
}

void BuildingManager::updateBuildings(float delta)
{
	// update all buildings every once in a while
	m_updateTimer += delta;
	if (m_updateTimer >= BUILDING_UPDATE_TIME)
	{
		m_updateTimer = 0;

		//m_game->decayTilePower();

		for (auto b : *m_buildings)
		{
			b->update();
			//b->updatePower();
		}

		m_game->clearTilePower();

		bool changedPower = true;
		while (changedPower)
		{
			changedPower = false;
			for (auto b : *m_buildings)
			{
				bool temp = b->updatePower();
				if (!changedPower)
					changedPower = temp;
			}
		}
	}
}

void BuildingManager::drawBuildings(aie::Renderer2D* renderer)
{
	for (auto b : *m_buildings)
	{
		renderer->setRenderColour(1, 1, 1);
		b->draw(renderer);
	}
}

void BuildingManager::addBuilding(Building* build)
{
	m_buildings->push_back(build);
	sortBuildings();
}

void BuildingManager::removeBuilding(Building* toRemove)
{
	int ix, iy;
	int iw, ih;
	toRemove->getPosition(&ix, &iy);
	toRemove->getSize(&iw, &ih);
	// make smokey particles on each tile it occupied
	for (int x = ix - iw; x <= ix; x++)
	{
		for (int y = iy - ih; y <= iy; y++)
		{
			// grab the world position
			float wx, wy;
			m_game->getTileWorldPosition(x, y, &wx, &wy);
			wx += TILE_WIDTH / 2.0f;
			m_game->spawnSmokeParticle(wx, wy);
		}
	}

	if (toRemove->getType() == BUILDINGTYPE_ROAD)
		m_game->getRoadManager()->removeRoad(toRemove);
	for (BuildingList::iterator it = m_buildings->begin();
		it != m_buildings->end(); ++it)
	{
		if (*it == toRemove)
		{
			delete *it;
			m_buildings->erase(it);
			break;
		}
	}
	sortBuildings();
}

void BuildingManager::sortBuildings()
{
	// sort buildings from back to front
	// todo: use a better sort algorithm
	int buildingCount = (int)m_buildings->size();
	bool changed = true;
	while (changed)
	{
		changed = false;
		for (int i = 0; i < buildingCount - 1; ++i)
		{
			for (int j = buildingCount - 1; j >= i + 1; --j)
			{
				// first building's position
				int ax, ay;
				(*m_buildings)[i]->getCenter(&ax, &ay);

				// and world position
				float awx, awy;
				m_game->getTileWorldPosition(ax, ay, &awx, &awy);

				// second building's position
				int bx, by;
				(*m_buildings)[i + 1]->getCenter(&bx, &by);
				// and world position
				float bwx, bwy;
				m_game->getTileWorldPosition(bx, by, &bwx, &bwy);

				if (bwy > awy)
				{
					// swap em
					Building* a = (*m_buildings)[i];
					(*m_buildings)[i] = (*m_buildings)[i + 1];
					(*m_buildings)[i + 1] = a;
					changed = true;
				}
			}
		}
	}
}

// returns whether or not the currently selected building can be placed at 
//   its position
bool BuildingManager::canPlaceBuilding()
{
	if (!m_ghostBuilding)
		return false;

	// grab building's position
	int buildingRight, buildingBottom;
	m_ghostBuilding->getPosition(&buildingRight, &buildingBottom);

	// the root position is outside of the world bounds
	if (buildingRight < 0 || buildingRight >= WORLD_WIDTH ||
		buildingBottom < 0 || buildingBottom >= WORLD_HEIGHT)
		return false;

	// get extremities(?) of this building
	int ghostSizeX, ghostSizeY;
	m_ghostBuilding->getSize(&ghostSizeX, &ghostSizeY);

	int buildingLeft = buildingRight - (ghostSizeX - 1);
	int buildingTop = buildingBottom - (ghostSizeY - 1);
	// some part of the building is outside of the world bounds
	if (buildingLeft < 0 || buildingLeft >= WORLD_WIDTH ||
		buildingTop < 0 || buildingTop >= WORLD_HEIGHT)
		return false;

	// now test against every other building
	for (auto b : *m_buildings)
	{
		// grab bounds of this building
		int right, bottom;
		b->getPosition(&right, &bottom);
		int width, height;
		b->getSize(&width, &height);
		// subtract 1 from size because a building with the size 1x1
		// will appear to be 2x2
		int left = right - (width - 1);
		int top = bottom - (height - 1);

		// check if they're intersecting
		if (!(buildingLeft > right || buildingRight < left ||
			buildingTop > bottom || buildingBottom < top))
		{
			return false;
		}
	}
	return true;
}

Building* BuildingManager::getBuildingAtIndex(int ix, int iy)
{
	// now test against every other building
	for (auto b : *m_buildings)
	{
		// grab bounds of this building
		int right, bottom;
		b->getPosition(&right, &bottom);
		int width, height;
		b->getSize(&width, &height);
		// subtract 1 from size because a building with the size 1x1
		// will appear to be 2x2
		int left = right - (width - 1);
		int top = bottom - (height - 1);

		// check if they're intersecting
		if (!(ix > right || ix < left ||
			iy > bottom || iy < top))
		{
			return b;
		}
	}
	return nullptr;
}

// creates a building with type type
// similar to a "factory"
// https://en.wikipedia.org/wiki/Factory_method_pattern
Building* BuildingManager::makeBuilding(BuildingType type, int xTile,
	int yTile, bool ghost)
{
	// in case we want to use the building we made for something else,
	// such as storing power poles in their vector
	Building* b;

	switch (type)
	{
	case BUILDINGTYPE_NONE:
		return nullptr;
		break;
	case BUILDINGTYPE_POWERPLANT:
		return new PowerPlant(m_game, xTile, yTile);
		break;
	case BUILDINGTYPE_POWERPOLE:
		return new PowerPole(m_game, xTile, yTile);
		break;
	case BUILDINGTYPE_ROAD:
		b = new Road(m_game, xTile, yTile);
		if (!ghost)
			m_game->getRoadManager()->addRoad(b);
		return b;
		break;
	default:
		printf("Tried to create a building that doesn't exist! Type: %d\n",
			(int)type);
		break;
	}
	return nullptr;
}
