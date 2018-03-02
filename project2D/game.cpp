////////////////////////////////////////////
// Game
// The main class of the game, in charge
//   of updating and drawing everything
//   as well as providing some gameplay
//   functionality
////////////////////////////////////////////

#include <ctime>
#include <functional>

#include <Texture.h>
#include <Font.h>
#include <Input.h>

#include "camera.h"
#include "game.h"
#include "uimanager.h"
#include "building.h"
#include "buildingmanager.h"
#include "roadmanager.h"
#include "zonemanager.h"

#include "tile.h"
#include "particle.h"
#include "smokeparticle.h"

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
	m_zoneManager = new ZoneManager(this, &m_tiles);

	//m_camera->setScale(1.3f);

	m_font = new aie::Font("./font/consolas.ttf", 32);
	m_uiFont = new aie::Font("./font/roboto.ttf", 16);
	m_uiFontLarge = new aie::Font("./font/roboto.ttf", 24);

	m_mapStartX = 1200;
	m_mapStartY = 800;

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
	setViewMode((ViewMode)(VIEWMODE_ZONE | VIEWMODE_BUILDINGS));

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

	delete m_buildingManager;
	delete m_roadManager;
	delete m_zoneManager;
	for (auto b : m_buildings)
		if (b)
			delete b;

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
			getZoneManager()->zoneMode();
			break;
		default:
			break;
		}
	}

	getBuildingManager()->updateBuildings(deltaTime);

	m_camera->update(deltaTime);
	getUiManager()->update(deltaTime);
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
	float mx, my;
	this->getMouseWorldPosition(&mx, &my);
	Tile* mouseOver = this->getTileAtPosition(mx, my);
	// don't show mouseover stuff if the mouse is over the UI
	if (!isMouseInGame())
		mouseOver = nullptr;
	// todo: replace with view mode
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
				rg = 0.5f;

			m_2dRenderer->setRenderColour(rg, rg, 1.0f);

			float xpos;
			float ypos;
			getTileWorldPosition(x, y, &xpos, &ypos);

			ypos -= 20.0f;

			// account for the difference in height in the texture
			// and keep the bottoms aligned
			float dify = TILE_HEIGHT -
				(float)thisTile->getTexture()->getHeight();

			// don't let the tile class tint the sprite if this is
			//   being moused over
			bool tintThisTile = tintTiles && thisTile != mouseOver;

			thisTile->draw(m_2dRenderer, xpos, ypos - dify / 2.0f, tintThisTile);
		}
	}

	// draw buildings
	if (isViewModeEnabled(VIEWMODE_BUILDINGS) || getPlaceMode() == PLACEMODE_BUILDING)
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
		getZoneManager()->drawZones(m_2dRenderer);
		break;
	default:
		break;
	}

	//tempPowerPlant->draw(m_2dRenderer);

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

	// show fps
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", getFPS());
	m_2dRenderer->setRenderColour(0, 0, 0);
	m_2dRenderer->drawText(m_uiFont, fps, 2, 6);

	// show screen's mouse position (as opposed to the world mouse position)
	m_2dRenderer->setRenderColour(1, 1, 1);
	m_2dRenderer->drawCircle((float)smx, (float)smy, 4);

	// done drawing hud
	m_2dRenderer->end();
}

void Game::getMouseWorldPosition(float* x, float* y)
{
	int mx, my;
	aie::Input::getInstance()->getMouseXY(&mx, &my);

	// convert to floats for camera translation
	float ax = (float)mx;
	float ay = (float)my;

	m_camera->screenToWorld(&ax, &ay);

	*x = ax;
	*y = ay;
}

bool Game::isMouseInGame()
{
	return !m_uiManager->isMouseOverUi();
}

// gets the tile at world position x,y 
Tile* Game::getTileAtPosition(float x, float y)
{
	int ix, iy;
	this->getTileAtPosition(x, y, &ix, &iy);

	if (ix < 0 || iy < 0)
		return nullptr;
	return m_tiles[iy][ix];
}

// gets the array indices of the tile at world position px,py and puts 
// them into ix,iy
void Game::getTileAtPosition(float px, float py, int* ix, int* iy)
{
	// get shorter names for the measurements we use to offset the tiles
	const float tw = TILE_WIDTH / 2.0f;
	const float th = TILE_HEIGHT / 3.0f;

	// invert y since y=0 is at the bottom
	py *= -1;

	// offset to the start of the map
	px -= m_mapStartX;
	py += m_mapStartY;

	// account for the origin of the tile drawing (see tile.cpp)
	px -= tw;
	py += th;

	int resultX = (int)((px / tw + py / th) / 2);
	int resultY = (int)((py / th - px / tw) / 2);

	if (resultX < 0 || resultX >= WORLD_WIDTH)
		resultX = -1;
	if (resultY < 0 || resultY >= WORLD_HEIGHT)
		resultY = -1;

	*ix = resultX;
	*iy = resultY;
}

void Game::getTileAtMousePosition(int* ix, int* iy)
{
	// first get the mouse's world position
	float mx, my;
	getMouseWorldPosition(&mx, &my);

	// then grab the tile's indices
	getTileAtPosition(mx, my, ix, iy);
}

void Game::getTileWorldPosition(int ix, int iy, float* ox, float* oy)
{
	float xpos = (ix - iy) * TILE_WIDTH / 2.0f + m_mapStartX;
	float ypos = (ix + iy) * TILE_HEIGHT / 3.0f - m_mapStartY;

	*ox = xpos;
	*oy = -ypos;
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


	// top-left tile
	float topLeftX, topLeftY;
	getTileWorldPosition(dragMinX, dragMinY,
		&topLeftX, &topLeftY);
	// top-right tile
	float topRightX, topRightY;
	getTileWorldPosition(dragMaxX + 1, dragMinY,
		&topRightX, &topRightY);
	// bottom-right tile
	float bottomRightX, bottomRightY;
	getTileWorldPosition(dragMaxX + 1, dragMaxY,
		&bottomRightX, &bottomRightY);
	// bottom-left tile
	float bottomLeftX, bottomLeftY;
	getTileWorldPosition(dragMinX, dragMaxY,
		&bottomLeftX, &bottomLeftY);

	// adjust so the lines line up with the borders of the tile sprites
	const float xOffset = TILE_WIDTH / 2.0f;
	const float yOffset = TILE_HEIGHT / 3.0f;
	topLeftX += xOffset;
	topLeftY += yOffset;
	topRightX += xOffset;
	topRightY += yOffset;

	const float lineThickness = 8.0f;
	// draw a rectangle out of lines
	m_2dRenderer->drawLine(topLeftX, topLeftY, topRightX, topRightY,
		lineThickness);
	m_2dRenderer->drawLine(topRightX, topRightY, bottomRightX, bottomRightY,
		lineThickness);
	m_2dRenderer->drawLine(bottomRightX, bottomRightY, bottomLeftX,
		bottomLeftY, lineThickness);
	m_2dRenderer->drawLine(bottomLeftX, bottomLeftY, topLeftX, topLeftY,
		lineThickness);
}

void Game::setPlaceMode(PlaceMode mode)
{
	m_placeMode = mode;
	getUiManager()->setShownPanel((int)mode);
}

bool Game::isViewModeEnabled(ViewMode mode)
{
	return (m_viewMode & mode);
}

void Game::spawnSmokeParticle(float x, float y)
{
	auto p = new SmokeParticle(this, x, y);
	m_particles.push_back(p);
}
