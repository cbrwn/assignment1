// all files follow this include layout:

// header related to this file
#include "game.h"

// standard includes
#include <ctime>

// bootstrap includes
#include "Font.h"
#include "Input.h"
#include "Texture.h"

// all other includes sorted by filename length
#include "tile.h"
#include "camera.h"
#include "darray.h"
#include "building.h"
#include "uimanager.h"
#include "roadmanager.h"
#include "savemanager.h"
#include "tilemanager.h"
#include "imagemanager.h"
#include "textparticle.h"
#include "smokeparticle.h"
#include "buildingmanager.h"
#include "pollutionparticle.h"

Game::Game() {}
Game::~Game() {}

bool Game::startup()
{
	srand((unsigned int)time(NULL));
	this->setVSync(true);

	// sky blue background
	setBackgroundColour(0.12f, 0.63f, 1.0f);

	m_buildings = new BuildingList;
	m_particles = new ParticleList;

	m_2dRenderer = new aie::Renderer2D();
	m_camera = new Camera(this);

	// I need a managers for all these managers
	m_imageManager = new ImageManager();
	m_uiManager = new UiManager(this);
	m_buildingManager = new BuildingManager(this, m_buildings);
	m_roadManager = new RoadManager(this);
	m_saveManager = new SaveManager(this);
	m_tileManager = new TileManager(this, &m_tiles);

	m_uiFont = new aie::Font("./font/roboto.ttf", 16);
	m_uiFontLarge = new aie::Font("./font/roboto.ttf", 24);

	// power icon used for the power viewmode
	m_powerIcon = m_imageManager->getTexture("icons/power");

	m_mapStart = Vector2(1200, 800);

	m_tiles = new Tile**[WORLD_HEIGHT];
	// initialize tiles to grass tiles
	for (int y = 0; y < WORLD_HEIGHT; ++y)
	{
		m_tiles[y] = new Tile*[WORLD_WIDTH];
		for (int x = 0; x < WORLD_WIDTH; ++x)
		{
			m_tiles[y][x] = new Tile(this,
				m_imageManager->getTexture("tiles/grass_flat"));
		}
	}

	m_placeMode = PlaceMode::PLACEMODE_NONE;
	// default view mode shows zones, buildings and roads
	setViewMode((ViewMode)(VIEWMODE_ZONE |
		VIEWMODE_BUILDINGS |
		VIEWMODE_ROADS));

	m_money = 2000;

	return true;
}

void Game::shutdown()
{
	delete m_uiFont;
	delete m_uiFontLarge;
	delete m_2dRenderer;
	delete m_camera;

	m_buildingManager->clearBuildings();
	delete m_buildingManager;
	delete m_roadManager;
	delete m_saveManager;
	delete m_tileManager;
	delete m_imageManager;
	delete m_uiManager;

	for (int i = 0; i < m_particles->getCount(); ++i)
		delete (*m_particles)[i];
	delete m_particles;
	delete m_buildings;

	for (int y = 0; y < WORLD_HEIGHT; ++y)
	{
		for (int x = 0; x < WORLD_WIDTH; ++x)
		{
			if (m_tiles[y][x] != nullptr)
				delete m_tiles[y][x];
		}
		// delete this column
		delete[] m_tiles[y];
	}
	// delete the array of columns
	delete[] m_tiles;
}

void Game::update(float deltaTime)
{
	// cap delta so it's doesn't super break if lag happens
	if (deltaTime > 0.33f)
		deltaTime = 0.33f;

	// update particles
	for (int i = 0; i < m_particles->getCount(); ++i)
		(*m_particles)[i]->update(deltaTime);

	// delete any particles that need it
	for (int i = m_particles->getCount() - 1; i >= 0; --i)
	{
		Particle* p = (*m_particles)[i];
		if (p->getOpacity() <= 0.0f)
		{
			delete p;
			m_particles->remove(i);
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

	// switching between view modes
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
	getBuildingManager()->updateBuildings(deltaTime);

	// temporary save/load keys
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

	// keys just to demonstrate these functions
	if (input->wasKeyPressed(aie::INPUT_KEY_J))
		m_saveManager->loadBuildings();
	if (input->wasKeyPressed(aie::INPUT_KEY_K))
		m_saveManager->saveBuildings();
	if (input->wasKeyPressed(aie::INPUT_KEY_G))
		m_saveManager->loadTiles();
	if (input->wasKeyPressed(aie::INPUT_KEY_H))
		m_saveManager->saveTiles();
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
	for (int y = 0; y < WORLD_HEIGHT; ++y)
	{
		for (int x = 0; x < WORLD_WIDTH; ++x)
		{
			Tile* thisTile = m_tiles[y][x];
			if (thisTile == nullptr)
				continue;
			// make sure the tile knows where it is in the array
			thisTile->setIndices(x, y);

			float rg = 1.0f;
			if (mouseOver == thisTile)
			{
				// set the tint colour
				rg = 0.5f;
				// and keep track of the mouseover location
				mouseOverX = x;
				mouseOverY = y;
			}
			// set the colour to a tint or full colour
			m_2dRenderer->setRenderColour(rg, rg, 1.0f);

			Vector2 tilePos = m_tileManager->getTileWorldPosition(x, y);
			// I'm not sure why I subtracted 20 here but let's go with it
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
	for (int i = 0; i < m_particles->getCount(); ++i)
		(*m_particles)[i]->draw(m_2dRenderer);

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

	// grab the screen coordinates of the mouse
	int smx, smy;
	aie::Input::getInstance()->getMouseXY(&smx, &smy);

	//===========
	// hud stuff
	//===========

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

		// temp pollution value
		char polValue[32];
		sprintf_s(polValue, 32, "%d", m_tileManager->getTile(mouseOverX, mouseOverY)->getPollution());
		m_2dRenderer->setRenderColour(1, 1, 1);
		m_2dRenderer->drawText(titleFont, polValue,
			mouseScreen.getX() + iconWidth, mouseScreen.getY() - iconHeight - titleHeight - 10);
	}

	// show fps
	char fps[32];
	sprintf_s(fps, 32, "FPS: %u", getFPS());
	m_2dRenderer->setRenderColour(0, 0, 0);
	m_2dRenderer->drawText(m_uiFont, fps, 2, 6);

	// show screen's mouse position (as opposed to the world mouse position)
	// just here because recording gifs on my hidpi monitor causes the pointer to
	//   show up in the wrong place
	m_2dRenderer->setRenderColour(1, 1, 1);
	m_2dRenderer->drawCircle((float)smx, (float)smy, 4);

	// done drawing hud
	m_2dRenderer->end();
}

// grabs the position of the mouse in world space
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

// grabs the position of the mouse in screen space
Vector2 Game::getMousePosition()
{
	int mx, my;
	aie::Input::getInstance()->getMouseXY(&mx, &my);
	return Vector2((float)mx, (float)my);
}


// whether or not the mouse should interact with the game
bool Game::isMouseInGame()
{
	return !m_uiManager->isMouseOverUi();
}

// draws a rectangle around tiles
void Game::drawTileRect(int left, int top, int right, int bottom)
{
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

	// grab the world positions of the bounds
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

// sets the camera's shakiness
void Game::doScreenShake(float amt)
{
	m_camera->setShakeAmount(amt);
	if (m_camera->getScale() >= amt / 9.0f)
		m_camera->setCurrentScale(m_camera->getTargetScale() - amt / 10.0f);
}

// -------------------------------
//   particle spawning functions:

void Game::spawnSmokeParticle(Vector2& pos)
{
	if (m_particles->getCount() >= MAX_PARTICLES)
		return;
	auto p = new SmokeParticle(this, pos);
	m_particles->add(p);
}

void Game::spawnPollutionParticle(Vector2& pos)
{
	if (m_particles->getCount() >= MAX_PARTICLES)
		return;
	auto p = new PollutionParticle(this, pos);
	m_particles->add(p);
}

void Game::spawnTextParticle(Vector2& pos, std::string text)
{
	if (m_particles->getCount() >= MAX_PARTICLES)
		return;
	auto p = new TextParticle(this, pos, text);
	m_particles->add(p);
}
