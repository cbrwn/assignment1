#include <ctime>
#include <functional>

#include <Texture.h>
#include <Font.h>
#include <Input.h>

#include "camera.h"
#include "game.h"
#include "imagemanager.h"
#include "uimanager.h"
#include "building.h"
#include "buildingmanager.h"
#include "roadmanager.h"
#include "savemanager.h"
#include "tilemanager.h"

#include "tile.h"
#include "particle.h"
#include "smokeparticle.h"

#include "road.h"

Game::Game()
{
}

Game::~Game()
{

}

bool Game::startup()
{
	srand((unsigned int)time(NULL));
	this->setVSync(true);


	// sky blue background
	setBackgroundColour(0.12f, 0.63f, 1.0f);

	m_2dRenderer = new aie::Renderer2D();
	m_camera = new Camera(this);
	m_imageManager = new ImageManager();
	m_uiManager = new UiManager(this);

	m_buildingManager = new BuildingManager(this, &m_buildings);
	m_roadManager = new RoadManager(this);
	m_saveManager = new SaveManager(this);
	m_tileManager = new TileManager(this, &m_tiles);

	//m_camera->setScale(1.3f);

	m_font = new aie::Font("./font/consolas.ttf", 32);
	m_uiFont = new aie::Font("./font/roboto.ttf", 16);
	m_uiFontLarge = new aie::Font("./font/roboto.ttf", 24);

	m_powerIcon = m_imageManager->getTexture("icons/power");

	m_mapStart = Vector2(1200, 800);

	m_tiles = new Tile**[WORLD_HEIGHT];

	// initialize tiles to grass tiles
	for (int y = 0; y < WORLD_HEIGHT; y++)
	{
		m_tiles[y] = new Tile*[WORLD_WIDTH];
		for (int x = 0; x < WORLD_WIDTH; x++)
			m_tiles[y][x] = new Tile(this,
				m_imageManager->getTexture("tiles/grass_flat"));
	}

	m_placeMode = PlaceMode::PLACEMODE_NONE;
	setViewMode((ViewMode)(VIEWMODE_ZONE |
		VIEWMODE_BUILDINGS |
		VIEWMODE_ROADS));

	m_money = 10000;

	return true;
}

void Game::shutdown()
{
	delete m_font;
	delete m_uiFont;
	delete m_uiFontLarge;
	delete m_2dRenderer;
	delete m_camera;

	delete m_imageManager;
	delete m_uiManager;

	m_buildingManager->clearBuildings();

	delete m_buildingManager;
	delete m_roadManager;
	delete m_saveManager;
	delete m_tileManager;

	for (auto p : m_particles)
		delete p;

	for (int y = 0; y < WORLD_HEIGHT; y++)
	{
		for (int x = 0; x < WORLD_WIDTH; x++)
		{
			if (m_tiles[y][x] != nullptr)
				delete m_tiles[y][x];
		}
		delete m_tiles[y];
	}
	delete[] m_tiles;
}

void Game::update(float deltaTime)
{
	if (deltaTime > 0.33f)
		deltaTime = 0.33f;

	// update particles
	for (auto p : m_particles)
		p->update(deltaTime);

	// delete any particles that need it
	bool didDelete = true;
	while (didDelete)
	{
		didDelete = false;
		for (ParticleList::iterator it = m_particles.begin();
			it != m_particles.end(); ++it)
		{
			Particle* thisParticle = *it;
			if (thisParticle && thisParticle->getOpacity() <= 0)
			{
				delete *it;
				m_particles.erase(it);
				didDelete = true;
				break;
			}
		}
	}

	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	// keys to switch between modes!

	// only want these to trigger if the mode isn't already what we want
	//   so it can fall through to the manager's update function to handle 
	//   turning it off
	bool switchToBuilding = m_placeMode != PLACEMODE_BUILDING
		&& input->wasKeyPressed(aie::INPUT_KEY_B);
	bool switchToZone = m_placeMode != PLACEMODE_ZONE
		&& input->wasKeyPressed(aie::INPUT_KEY_Z);

	if (switchToBuilding)
		setPlaceMode(PLACEMODE_BUILDING);
	else if (switchToZone)
		setPlaceMode(PLACEMODE_ZONE);
	else
	{
		switch (m_placeMode)
		{
		case PLACEMODE_BUILDING:
			getBuildingManager()->buildingMode();
			break;
		case PLACEMODE_ZONE:
			getTileManager()->updateZoneEditing();
			break;
		default:
			break;
		}
	}

	getBuildingManager()->updateBuildings(deltaTime);

	if (input->wasKeyPressed(aie::INPUT_KEY_P))
		toggleViewMode(VIEWMODE_POWER);
	if (input->wasKeyPressed(aie::INPUT_KEY_O))
		toggleViewMode(VIEWMODE_ZONE);
	if (input->wasKeyPressed(aie::INPUT_KEY_I))
		toggleViewMode(VIEWMODE_BUILDINGS);
	if (input->wasKeyPressed(aie::INPUT_KEY_U))
		toggleViewMode(VIEWMODE_ROADS);

	m_camera->update(deltaTime);
	getUiManager()->update(deltaTime);

	if (input->wasKeyPressed(aie::INPUT_KEY_F))
	{
		if (getSaveManager()->saveData())
			printf("Saved successfully!\n");
		else
			printf("Something went wrong when saving!\n");
	}

	if (input->wasKeyPressed(aie::INPUT_KEY_D))
	{
		if (getSaveManager()->loadData())
			printf("Loaded successfully!\n");
		else
			printf("Something went wrong when loading!\n");
	}
}

void Game::draw()
{
	// wipe the screen to the background colour
	clearScreen();

	// set the camera position before we begin rendering
	float camX, camY;
	m_camera->getPosition(&camX, &camY);
	m_2dRenderer->setCameraPos(camX, camY);
	m_2dRenderer->setCameraScale(m_camera->getScale());

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw tiles
	Vector2 mousePos = getMouseWorldPosition();
	Tile* mouseOver = m_tileManager->getTileAtPosition(mousePos);
	int mouseOverX, mouseOverY;
	// don't show mouseover stuff if the mouse is over the UI
	if (!isMouseInGame())
		mouseOver = nullptr;

	// only show zone tint when it's relevant
	bool tintTiles = getPlaceMode() == PLACEMODE_ZONE
		|| isViewModeEnabled(VIEWMODE_ZONE);
	for (int y = 0; y < WORLD_HEIGHT; y++)
	{
		for (int x = 0; x < WORLD_WIDTH; x++)
		{
			Tile* thisTile = m_tiles[y][x];
			if (thisTile == nullptr)
				continue;


			float rg = 1.0f;
			if (mouseOver == thisTile)
			{
				rg = 0.5f;
				mouseOverX = x;
				mouseOverY = y;
			}

			m_2dRenderer->setRenderColour(rg, rg, 1.0f);

			Vector2 tilePos = m_tileManager->getTileWorldPosition(x, y);

			tilePos.setY(tilePos.getY() - 20.0f);

			// account for the difference in height in the texture
			// and keep the bottoms aligned
			float dify = TILE_HEIGHT -
				(float)thisTile->getTexture()->getHeight();

			// don't let the tile class tint the sprite if this is
			//   being moused over
			bool tintThisTile = tintTiles && thisTile != mouseOver;

			thisTile->draw(m_2dRenderer, tilePos.getX(),
				tilePos.getY() - dify / 2.0f, tintThisTile);

			// draw power icon if needed
			if (isViewModeEnabled(VIEWMODE_POWER) && thisTile->hasPower())
			{
				m_2dRenderer->setRenderColour(1, 1, 0);
				m_2dRenderer->drawSprite(m_powerIcon,
					tilePos.getX() + TILE_WIDTH / 2.0f, tilePos.getY() + 20.0f);
			}
		}
	}

	// draw buildings
	getBuildingManager()->drawBuildings(m_2dRenderer);

	// draw particles
	for (auto p : m_particles)
		p->draw(m_2dRenderer);

	// draw overlay stuff from place mode
	switch (m_placeMode)
	{
	case PLACEMODE_BUILDING:
		getBuildingManager()->drawPlacement(m_2dRenderer);
		break;
	case PLACEMODE_ZONE:
		getTileManager()->drawZoneSelection(m_2dRenderer);
		break;
	default:
		break;
	}

	// done drawing sprites
	m_2dRenderer->end();

	// set camera back to 0,0 for ui stuff
	m_2dRenderer->setCameraPos(0, 0);
	m_2dRenderer->setCameraScale(1.0f);

	// start drawing ui
	m_2dRenderer->begin();

	int smx, smy;
	aie::Input::getInstance()->getMouseXY(&smx, &smy);

	//===========
	// hud stuff
	//===========

	// show money
	char mny[32];
	sprintf_s(mny, 32, "$%d", m_money);
	float moneyWidth = m_uiFontLarge->getStringWidth(mny);
	m_2dRenderer->setRenderColour(0, 0.4f, 0);
	m_2dRenderer->drawText(m_uiFontLarge, mny,
		getWindowWidth() - moneyWidth - 2, (float)getWindowHeight() - 18);

	getUiManager()->draw(m_2dRenderer);

	// draw mouseover stuff if we need
	Building* b = nullptr;
	if (mouseOver)
		b = m_buildingManager->getBuildingAtIndex(mouseOverX, mouseOverY);
	if (b)
	{
		// draw mouseover stuff
		// grab the screen position of the mouse
		Vector2 mouseScreen = getMousePosition();

		// some constants about the mouseover box
		const int boxOffsetX = 10; // x offset from mouse
		const int boxOffsetY = 10; // y offset from mouse
		const int boxPadding = 8;

		aie::Font* titleFont = m_uiFontLarge;

		// grab info to do with the text
		char* buildingName = Building::buildingNames[b->getType()];
		float titleWidth = titleFont->getStringWidth(buildingName);
		float titleHeight = titleFont->getStringHeight(buildingName);

		// and the power icon
		const int iconHeight = m_powerIcon->getHeight();
		const int iconWidth = m_powerIcon->getWidth();

		// info about the box itself
		float boxWidth = titleWidth + (boxPadding * 2.0f);
		float boxHeight = titleHeight + (boxPadding * 2.0f) + iconHeight;
		float boxX = mouseScreen.getX() + boxWidth / 2.0f;
		float boxY = mouseScreen.getY() - boxHeight / 2.0f;

		m_2dRenderer->setRenderColour(0.2f, 0.2f, 0.3f, 0.8f);
		m_2dRenderer->drawBox(boxX, boxY, boxWidth, boxHeight);

		m_2dRenderer->setRenderColour(1, 1, 1);
		m_2dRenderer->drawText(titleFont, buildingName,
			mouseScreen.getX() + boxPadding, mouseScreen.getY() - boxPadding - titleHeight);

		// power icon
		if (mouseOver->hasPower())
			m_2dRenderer->setRenderColour(0, 1, 0);
		else
			m_2dRenderer->setRenderColour(1, 0, 0);
		m_2dRenderer->drawSprite(m_powerIcon, mouseScreen.getX() + iconWidth / 2.0f,
			mouseScreen.getY() - iconHeight - titleHeight);
	}

	// show fps
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", getFPS());
	m_2dRenderer->setRenderColour(0, 0, 0);
	m_2dRenderer->drawText(m_uiFont, fps, 2, 6);

	// show demand graph
	const float demandStartX = (float)getWindowWidth() - 32;
	const float demandStartY = (float)getWindowHeight() - 64;
	const unsigned int demandColours[3] = {
		0x00ff00ff,
		0x0000ffff,
		0xffff00ff
	};
	const char demandInitials[3] = {
		'R','C','I'
	};
	for (int i = 0; i < 3; i++)
	{
		ZoneType type = (ZoneType)(i + 1);
		float zoneDemand = m_buildingManager->getDemand(type) - 1.0f;
		zoneDemand *= 10.0f;
		m_2dRenderer->setRenderColour(demandColours[i]);

		float boxX = demandStartX + i * 10;
		float boxY = demandStartY;

		m_2dRenderer->drawBox(boxX, boxY + zoneDemand / 2.0f, 8, zoneDemand);
		m_2dRenderer->setRenderColour(1, 1, 1);
		char initial[2] = { demandInitials[i], 0 };
		m_2dRenderer->drawText(m_uiFont, initial, boxX - 4, boxY - 12.0f);
	}

	// show screen's mouse position (as opposed to the world mouse position)
	// just here because recording gifs on my hidpi monitor causes the pointer to
	//   show up in the wrong place
	m_2dRenderer->setRenderColour(1, 1, 1);
	m_2dRenderer->drawCircle((float)smx, (float)smy, 4);

	// done drawing hud
	m_2dRenderer->end();
}

Vector2 Game::getMouseWorldPosition()
{
	int mx, my;
	aie::Input::getInstance()->getMouseXY(&mx, &my);

	// convert to floats for camera translation
	float ax = (float)mx;
	float ay = (float)my;

	m_camera->screenToWorld(&ax, &ay);
	return Vector2(ax, ay);
}

Vector2 Game::getMousePosition()
{
	int mx, my;
	aie::Input::getInstance()->getMouseXY(&mx, &my);
	return Vector2((float)mx, (float)my);
}

bool Game::isMouseInGame()
{
	return !m_uiManager->isMouseOverUi();
}

void Game::drawTileRect(int left, int top, int right, int bottom)
{
	// grab the world positions of the tiles
	// -------------------------------------

	// swap the start/end if the rectangle is backwards
	int dragMinX = left; int dragMinY = top;
	int dragMaxX = right; int dragMaxY = bottom;
	if (dragMinX > dragMaxX)
	{
		dragMinX = right;
		dragMaxX = left;
	}
	if (dragMinY > dragMaxY)
	{
		dragMinY = bottom;
		dragMaxY = top;
	}

	Vector2 topLeft =
		m_tileManager->getTileWorldPosition(dragMinX, dragMinY);
	Vector2 topRight =
		m_tileManager->getTileWorldPosition(dragMaxX + 1, dragMinY);
	Vector2 bottomRight =
		m_tileManager->getTileWorldPosition(dragMaxX + 1, dragMaxY);
	Vector2 bottomLeft =
		m_tileManager->getTileWorldPosition(dragMinX, dragMaxY);

	// adjust so the lines line up with the borders of the tile sprites
	const float xOffset = TILE_WIDTH / 2.0f;
	const float yOffset = TILE_HEIGHT / 3.0f;
	topLeft.setX(topLeft.getX() + xOffset);
	topLeft.setY(topLeft.getY() + yOffset);
	topRight.setX(topRight.getX() + xOffset);
	topRight.setY(topRight.getY() + yOffset);

	const float lineThickness = 8.0f;
	// draw a rectangle out of lines
	m_2dRenderer->drawLine(topLeft.getX(), topLeft.getY(), topRight.getX(),
		topRight.getY(), lineThickness);
	m_2dRenderer->drawLine(topRight.getX(), topRight.getY(), bottomRight.getX(),
		bottomRight.getY(), lineThickness);
	m_2dRenderer->drawLine(bottomRight.getX(), bottomRight.getY(), bottomLeft.getX(),
		bottomLeft.getY(), lineThickness);
	m_2dRenderer->drawLine(bottomLeft.getX(), bottomLeft.getY(), topLeft.getX(),
		topLeft.getY(), lineThickness);
}

void Game::setPlaceMode(PlaceMode mode)
{
	m_placeMode = mode;
	getUiManager()->setShownPanel((int)mode);
}

void Game::toggleViewMode(ViewMode mode)
{
	if (isViewModeEnabled(mode))
	{
		// clear the bit
		int newMode = m_viewMode & ~(mode);
		m_viewMode = (ViewMode)newMode;
	}
	else
	{
		// turn the bit on
		m_viewMode = (ViewMode)(m_viewMode | mode);
	}
}

void Game::spawnSmokeParticle(Vector2& pos)
{
	auto p = new SmokeParticle(this, pos);
	m_particles.push_back(p);
}

// adds amt to the current shake amount
void Game::doScreenShake(float amt)
{
	m_camera->setShakeAmount(amt);
}
