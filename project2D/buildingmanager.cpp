#include <iostream>
#include <Input.h>
#include <algorithm>
#include <chrono>

#include "buildingmanager.h"
#include "roadmanager.h"
#include "game.h"
#include "tile.h"
#include "tilemanager.h"
#include "random.h"
#include "uimanager.h"

#include "building.h"
#include "factory.h"
#include "house.h"
#include "powerplant.h"
#include "powerpole.h"
#include "road.h"
#include "shop.h"

BuildingManager::BuildingManager(Game* game, BuildingList* buildings)
	: m_game(game), m_buildings(buildings)
{
	m_selectedBuilding = 0;
	m_powerTimer = 0;
	m_houseTimer = 0;
	m_ghostBuilding = nullptr;

	m_dragging = false;
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
	Vector2 mousePos = m_game->getMouseWorldPosition();

	// get the index of the tile we're mousing over
	int tileX, tileY;
	m_game->getTileManager()->getTileAtPosition(mousePos, &tileX, &tileY);

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
		&& input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_LEFT)
		&& m_game->isMouseInGame())
	{
		Building* underMouse = getBuildingAtIndex(tileX, tileY);
		if (underMouse)
			removeBuilding(underMouse);
	}

	if (m_selectedBuilding == BUILDINGTYPE_NONE)
		return;

	// line style lets you click and drag
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_LEFT)
		&& m_ghostBuilding->getBuildStyle() == BuildStyle::BUILDSTYLE_LINE)
	{
		if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT)
			&& m_game->isMouseInGame() && tileX > -1 && tileY > -1)
		{
			// dragging start
			m_dragStartX = tileX;
			m_dragStartY = tileY;
			m_dragging = true;
		}

		if (m_dragging)
		{
			// update the end point of the line so we can draw it
			m_dragPosX = tileX;
			m_dragPosY = tileY;

			// check if the line we're drawing is horizontal or vertical
			// based on the direction we're dragging
			int horizontalDist = (int)fabsf((float)m_dragStartX - m_dragPosX);
			int verticalDist = (int)fabsf((float)m_dragStartY - m_dragPosY);
			m_isDragHorizontal = horizontalDist > verticalDist;

			if (m_isDragHorizontal)
				m_dragPosY = m_dragStartY;
			else
				m_dragPosX = m_dragStartX;
		}
	}
	else if (m_dragging)
	{
		m_dragging = false;

		float altitude = 10000.0f;

		// keep track of if we've placed something so we don't sort/update
		//   when we don't need to
		bool placed = false;

		// get the direction we should spawn them in
		int signX = (m_dragPosX - m_dragStartX) < 0 ? -1 : 1;
		int signY = (m_dragPosY - m_dragStartY) < 0 ? -1 : 1;

		// strange stuff in the for loop to spawn them in the direction
		//   that the player dragged
		for (int y = m_dragStartY; y != m_dragPosY + signY; y += signY)
		{
			for (int x = m_dragStartX; x != m_dragPosX + signX; x += signX)
			{
				if (getBuildingAtIndex(x, y))
					continue;

				Building* newBuilding =
					makeBuilding(m_ghostBuilding->getType(), x, y);
				newBuilding->setAltitude(altitude);
				placeBuilding(newBuilding, false);

				altitude += 500.0f;

				if (!placed)
					placed = true;
			}
		}
		if (placed)
		{
			sortBuildings();
			if (m_ghostBuilding->getType() == BUILDINGTYPE_ROAD)
				m_game->getRoadManager()->updateRoads();
		}
	}

	if (!(canPlaceBuilding() && m_game->isMouseInGame()))
		return;

	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT)
		&& m_ghostBuilding->getBuildStyle() == BuildStyle::BUILDSTYLE_SINGLE)
	{
		// place building!
		placeBuilding(makeBuilding((BuildingType)m_selectedBuilding,
			tileX, tileY));
		// cancel building mode if shift isn't held
		// so player can place multiple buildings easily by holding shift
		if (!input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT))
			m_game->setPlaceMode(PlaceMode::PLACEMODE_NONE);
	}
}

// draws everything while in building mode
// basically just the translucent ghost building to show the player
//   what they're building
void BuildingManager::drawPlacement(aie::Renderer2D* renderer)
{
	if (m_ghostBuilding)
	{
		// draw the range of the power grabbygrab
		if (m_ghostBuilding->getPowerSearch() > 0)
		{
			int sizeX, sizeY;
			m_ghostBuilding->getSize(&sizeX, &sizeY);
			sizeX--; sizeY--;
			int posX, posY;
			m_ghostBuilding->getPosition(&posX, &posY);

			// grab the bounds of the rectangle we're going to show
			int left = posX - sizeX - m_ghostBuilding->getPowerSearch();
			int right = posX + m_ghostBuilding->getPowerSearch();
			int top = posY - sizeY - m_ghostBuilding->getPowerSearch();
			int bottom = posY + m_ghostBuilding->getPowerSearch();

			renderer->setRenderColour(1, 1, 0, 0.2f);
			m_game->drawTileRect(left, top, right, bottom);
		}

		const float alpha = 0.4f;
		// tint green if we can place
		if (canPlaceBuilding())
			renderer->setRenderColour(0, 1, 0, alpha);
		else // and red if not
			renderer->setRenderColour(1, 0, 0, alpha);
		m_ghostBuilding->draw(renderer);
	}

	// draw dragging selection
	if (m_dragging)
	{
		renderer->setRenderColour(1, 1, 1);
		m_game->drawTileRect(m_dragStartX, m_dragStartY, m_dragPosX, m_dragPosY);
	}
}

void BuildingManager::updateBuildings(float delta)
{
	// the regular update function deals with per-frame stuff
	for (auto b : *m_buildings)
		b->update(delta);

	// update everything else that needs to be updated once
	//   every so often
	m_powerTimer += delta;
	if (m_powerTimer >= POWER_UPDATE_TIME)
	{

		m_game->getTileManager()->clearTilePower();
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

		m_powerTimer = 0;
	}

	// update houses
	m_houseTimer -= delta;
	if (m_houseTimer <= 0)
	{
		// get demand values for use in the loop

		int newBuildings = 0;

		auto tmr = std::chrono::high_resolution_clock::now();

		// grab a list of all tiles which could use buildings
		std::vector<Tile*> zonedTiles;

		for (int y = 0; y < WORLD_HEIGHT; ++y)
		{
			for (int x = 0; x < WORLD_WIDTH; ++x)
			{
				Tile* t = m_game->getTileManager()->getTile(x, y);
				if (t->getZoneType() == ZONETYPE_NONE)
					continue;
				zonedTiles.push_back(t);
			}
		}
		std::random_shuffle(zonedTiles.begin(), zonedTiles.end());
		int tileAmt = (int)zonedTiles.size();


		int processed = 0;
		int toProcess = (tileAmt / 10) + 1;

			float resDemand = getResidentialDemand();
		float comDemand = getCommercialDemand();
		float indDemand = getIndustrialDemand();

		for (auto t : zonedTiles)
		{
			if (processed >= toProcess)
				break;

			int xIndex, yIndex;
			t->getIndices(&xIndex, &yIndex);

			if (getBuildingAtIndex(xIndex, yIndex))
				continue;

			if (!t->isLiveable())
				continue;

			Building* newBuilding = nullptr;

			const float minDemand = 1.0f;

			switch (t->getZoneType())
			{
			case ZONETYPE_RESIDENTIAL:
				if (resDemand < minDemand)
					continue;
				newBuilding = makeBuilding(BUILDINGTYPE_HOUSE, 
					xIndex, yIndex);
				break;
			case ZONETYPE_COMMERCIAL:
				if (comDemand < minDemand)
					continue;
				newBuilding = makeBuilding(BUILDINGTYPE_SHOP, 
					xIndex, yIndex);
				break;
			case ZONETYPE_INDUSTRIAL:
				if (indDemand < minDemand)
					continue;
				newBuilding = makeBuilding(BUILDINGTYPE_FACTORY, 
					xIndex, yIndex);
				break;
			}

			if (newBuilding)
			{
				newBuilding->setAltitude((float)randBetween(1000, 5000));
				addBuilding(newBuilding, false);
				newBuildings++;
				processed++;
			}
		}
		auto t2 = std::chrono::high_resolution_clock::now();
		auto d = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - tmr);

		printf("building processing: %lld\n", d.count());

		if (newBuildings > 0)
			sortBuildings();

		// remove invalid houses
		for (auto b : *m_buildings)
		{
			if (randBetween(0, 100) > 25)
				continue;
			if (b->getType() != BUILDINGTYPE_HOUSE &&
				b->getType() != BUILDINGTYPE_FACTORY &&
				b->getType() != BUILDINGTYPE_SHOP)
				continue;
			int ix, iy;
			b->getPosition(&ix, &iy);

			Tile* underHouse = m_game->getTileManager()->getTile(ix, iy);

			// check if the zone type matches the building
			bool zoneMatches = false;
			switch (b->getType())
			{
			case BUILDINGTYPE_HOUSE:
				zoneMatches = underHouse->getZoneType() == ZONETYPE_RESIDENTIAL;
				break;
			case BUILDINGTYPE_SHOP:
				zoneMatches = underHouse->getZoneType() == ZONETYPE_COMMERCIAL;
				break;
			case BUILDINGTYPE_FACTORY:
				zoneMatches = underHouse->getZoneType() == ZONETYPE_INDUSTRIAL;
				break;
			}

			if (underHouse->isLiveable() && zoneMatches)
				continue;

			// delete this invalid house
			removeBuilding(b);
		}

		// make money from buildings
		int factoryCount = getBuildingCount(BUILDINGTYPE_FACTORY);
		int shopCount = getBuildingCount(BUILDINGTYPE_SHOP);

		const float moneyPerFactory = 5.0f;
		const float moneyPerShop = 1.0f;

		float newMoney = moneyPerFactory * factoryCount;
		newMoney += moneyPerShop * shopCount;
		m_game->addMoney((int)newMoney);

		m_houseTimer = HOUSE_UPDATE_TIME;// +(randBetween(-1.0f, 1.0f) * HOUSE_UPDATE_TIME * 0.5f);
	}
}

void BuildingManager::drawBuildings(aie::Renderer2D* renderer)
{
	for (auto b : *m_buildings)
	{
		// if building view is off, we want to skip everything except roads
		if (!m_game->isViewModeEnabled(VIEWMODE_BUILDINGS)
			&& b->getType() != BUILDINGTYPE_ROAD)
			continue;
		// skip roads if road view is off
		if (!m_game->isViewModeEnabled(VIEWMODE_ROADS)
			&& b->getType() == BUILDINGTYPE_ROAD)
			continue;
		renderer->setRenderColour(1, 1, 1);
		b->draw(renderer);
	}
}

void BuildingManager::addBuilding(Building* build, bool sort)
{
	int posX, posY;
	build->getPosition(&posX, &posY);
	if (posX <= -1 || posY <= -1)
	{
		delete build;
		return;
	}

	m_buildings->push_back(build);
	if (sort)
		sortBuildings();
	if (build->getType() == BUILDINGTYPE_ROAD)
		m_game->getRoadManager()->addRoad(build, sort);
}

// called when the player places a building
void BuildingManager::placeBuilding(Building* build, bool sort)
{
	int buildingPrice = build->getPrice();
	if (m_game->getMoney() < buildingPrice)
	{
		// oh no you're too poor
		// so we'll turn the money red so you know how poor you are
		m_game->getUiManager()->flashMoney();

		// and then get rid of your building
		delete build;
		// you disgusting poor person
		return;
	}

	addBuilding(build, sort);
	m_game->addMoney(-build->getPrice());

	char ptext[16];
	sprintf_s(ptext, 16, "-$%d", build->getPrice());

	Vector2 spawnPos = build->getWorldPosition();
	m_game->spawnTextParticle(spawnPos, ptext);
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
			Vector2 tPos =
				m_game->getTileManager()->getTileWorldPosition(x, y);
			tPos.setX(tPos.getX() + TILE_WIDTH / 2.0f);
			m_game->spawnSmokeParticle(tPos);
		}
	}

	// roads need to be deleted from the road manager
	if (toRemove->getType() == BUILDINGTYPE_ROAD)
		m_game->getRoadManager()->removeRoad(toRemove);

	// remove from list and delete
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
}

void BuildingManager::sortBuildings()
{
	TileManager* tm = m_game->getTileManager();

	std::sort(m_buildings->begin(), m_buildings->end(), [tm](Building* a, Building* b)
	{
		int aCenterX, aCenterY;
		a->getCenter(&aCenterX, &aCenterY);
		Vector2 aPos = tm->getTileWorldPosition(aCenterX, aCenterY);

		int bCenterX, bCenterY;
		b->getCenter(&bCenterX, &bCenterY);
		Vector2 bPos = tm->getTileWorldPosition(bCenterX, bCenterY);

		return aPos.getY() > bPos.getY();
	});
}

float BuildingManager::getDemand(ZoneType zone)
{
	// send this off so other functions
	// because I can't declare variables in switch statements :(

	float result = 0.0f;

	switch (zone)
	{
	case ZONETYPE_RESIDENTIAL:
		result = getResidentialDemand();
		break;
	case ZONETYPE_COMMERCIAL:
		result = getCommercialDemand();
		break;
	case ZONETYPE_INDUSTRIAL:
		result = getIndustrialDemand();
		break;
	default:
		return -1.0f;
		break;
	}

	// raise the result to a silly power
	// to make the differences more apparent but still comparatively the same
	result = pow(result, 100);

	// limit this result
	const int maxDemand = 5;
	if (result > maxDemand)
		result = maxDemand;
	if (result < -maxDemand)
		result = -maxDemand;

	return result;
}

const float factorySpace = 2.0f;
const float shopSpace = 3.0f;

float BuildingManager::getResidentialDemand()
{
	/*
	residents will not want to move in if:
	- there aren't enough shops
	- there aren't enough factories(jobs)
	*/
	int houseCount = getBuildingCount(BUILDINGTYPE_HOUSE);
	int shopCount = getBuildingCount(BUILDINGTYPE_SHOP);
	int factoryCount = getBuildingCount(BUILDINGTYPE_FACTORY);

	// there will always be demand if no houses exist
	// this is to ensure there's at least enough to get started
	if (houseCount <= 0)
		return 10.0f;

	float shopsPerResident = (shopCount / (float)houseCount) * shopSpace;
	float jobsPerResident = factoryCount / (houseCount / factorySpace);

	return (shopsPerResident + jobsPerResident) / 2.0f;
}
float BuildingManager::getCommercialDemand()
{
	/*
	shops will not want to build if:
	- there aren't enough residents
	*/
	int houseCount = getBuildingCount(BUILDINGTYPE_HOUSE);
	int shopCount = getBuildingCount(BUILDINGTYPE_SHOP);
	if (shopCount <= 0)
		return 10.0f;

	float residentsPerShop = houseCount / (shopCount * shopSpace);
	return residentsPerShop;
}
float BuildingManager::getIndustrialDemand()
{
	/*
	factories will not want to build if:
	- there aren't enough workers
	*/
	int houseCount = getBuildingCount(BUILDINGTYPE_HOUSE);
	int factoryCount = getBuildingCount(BUILDINGTYPE_FACTORY);
	if (factoryCount <= 0)
		return 10.0f;

	float residentsPerJob = (houseCount / factorySpace) / (float)factoryCount;
	return residentsPerJob;
}

int BuildingManager::getBuildingCount(BuildingType type)
{
	int result = 0;
	for (auto b : *m_buildings)
	{
		if (b->getType() == type)
			result++;
	}
	return result;
}

// returns whether or not the currently selected building can be placed at 
//   its position
bool BuildingManager::canPlaceBuilding()
{
	if (!m_ghostBuilding)
		return false;

	// check its price
	int thisPrice = m_ghostBuilding->getPrice();
	int currentMoney = m_game->getMoney();
	if (currentMoney < thisPrice)
	{
		m_game->getUiManager()->flashMoney();
		return false;
	}

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

void BuildingManager::clearBuildings()
{
	for (auto b : *m_buildings)
		if (b)
			delete b;
	m_buildings->clear();
}

// creates a building with type type
// similar to a "factory"
// https://en.wikipedia.org/wiki/Factory_method_pattern
Building* BuildingManager::makeBuilding(BuildingType type, int xTile,
	int yTile, bool ghost)
{
	// in case we want to use the building we made for something else,
	// such as storing power poles in their vector
	Building* b = nullptr;

	switch (type)
	{
	case BUILDINGTYPE_NONE:
		return nullptr;
		break;
	case BUILDINGTYPE_POWERPLANT:
		b = new PowerPlant(m_game, xTile, yTile);
		break;
	case BUILDINGTYPE_POWERPOLE:
		b = new PowerPole(m_game, xTile, yTile);
		break;
	case BUILDINGTYPE_ROAD:
		b = new Road(m_game, xTile, yTile);
		break;
	case BUILDINGTYPE_HOUSE:
		b = new House(m_game, xTile, yTile);
		break;
	case BUILDINGTYPE_SHOP:
		b = new Shop(m_game, xTile, yTile);
		break;
	case BUILDINGTYPE_FACTORY:
		b = new Factory(m_game, xTile, yTile);
		break;
	default:
		printf("Tried to create a building that doesn't exist! Type: %d\n",
			(int)type);
		break;
	}

	if (ghost)
		b->setAltitude(0);

	return b;
}
