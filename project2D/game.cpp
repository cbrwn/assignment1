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

#include "tile.h"

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

	//m_camera->setScale(1.3f);

	m_font = new aie::Font("./font/consolas.ttf", 32);
	m_uiFont = new aie::Font("./font/roboto.ttf", 16);
	m_uiFontLarge = new aie::Font("./font/roboto.ttf", 24);

	// initialize tiles to grass tiles
	m_mapStartX = 1200;
	m_mapStartY = 800;
	for (int y = 0; y < WORLD_HEIGHT; y++)
		for (int x = 0; x < WORLD_WIDTH; x++)
			m_tiles[y][x] = new Tile(this, m_imageManager->getTexture("tiles/grass_flat"));

	m_placeMode = PlaceMode::NONE;

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
	for (auto b : m_buildings)
		if(b)
			delete b;

	for (int y = 0; y < WORLD_HEIGHT; y++)
		for (int x = 0; x < WORLD_WIDTH; x++)
			if (m_tiles[y][x] != nullptr)
				delete m_tiles[y][x];
}

void Game::update(float deltaTime)
{
	// input example
	aie::Input* input = aie::Input::getInstance();

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT) &&
		isMouseInGame())
	{
		float mx, my;
		this->getMouseWorldPosition(&mx, &my);
		Tile* mouseOver = getTileAtPosition(mx, my);
		if (mouseOver != nullptr)
		{
			tileClicked(mouseOver);
		}
	}

	switch (m_placeMode)
	{
	case BUILDING:
		getBuildingManager()->buildingMode();
		break;
	case ZONE:
		break;
	default:
		if (input->wasKeyPressed(aie::INPUT_KEY_B))
			setPlaceMode(PlaceMode::BUILDING);
		break;
	}

	getBuildingManager()->updateBuildings(deltaTime);

	m_camera->update(deltaTime);
	m_uiManager->update(deltaTime);
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
			float dify = TILE_HEIGHT - (float)thisTile->getTexture()->getHeight();

			thisTile->draw(m_2dRenderer, xpos, ypos - dify / 2.0f);
		}
	}

	// draw buildings
	getBuildingManager()->drawBuildings(m_2dRenderer);

	// draw overlay stuff from place mode
	switch (m_placeMode)
	{
	case BUILDING:
		getBuildingManager()->draw(m_2dRenderer);
		break;
	case ZONE:
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

	// draw mouseover information
	/*if (mouseOver != nullptr)
	{
		char* info = mouseOver->getMouseoverText();
		// change the font here if needed
		aie::Font* mouseOverFont = m_uiFont;
		if (strlen(info) > 0)
		{
			int mx = smx + 10;
			int my = smy - 20;

			// get string size to draw the rectangle appropriately
			float stringWidth = mouseOverFont->getStringWidth(info);
			float stringHeight = mouseOverFont->getStringHeight(info);
			// number of pixels of padding on each side of the text
			const float padding = 4.0f;

			// 2*padding to put padding on both sides
			float boxWidth = stringWidth + (padding * 2.0f);
			// 2* stringHeight to make room for the tile name too
			float boxHeight = stringHeight * 2.0f + (padding * 2.0f);

			// draw our box
			m_2dRenderer->setRenderColour(0.33f, 0.33f, 0.33f);
			m_2dRenderer->drawBox((float)mx + boxWidth / 2.0f, (float)my - boxHeight / 2.0f + stringHeight,
				boxWidth, boxHeight);

			// and then our text
			// tile name first
			m_2dRenderer->setRenderColour(1, 1, 0);
			m_2dRenderer->drawText(mouseOverFont, m_tileNames[mouseOver->getType()],
				(float)mx + padding, (float)my);

			// and the mouseover info
			m_2dRenderer->setRenderColour(1, 1, 1);
			m_2dRenderer->drawText(mouseOverFont, info,
				(float)mx + padding, (float)my - padding - stringHeight);

			delete info;
		}
	}*/

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

	m_uiManager->draw(m_2dRenderer);

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

void Game::updateTiles()
{
	for (int y = 0; y < WORLD_HEIGHT; y++)
	{
		for (int x = 0; x < WORLD_WIDTH; x++)
		{
			Tile* thisTile = m_tiles[y][x];
			if (thisTile == nullptr)
				continue;
			thisTile->update();
		}
	}
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

// gets the array indices of the tile at world position px,py and puts them into ix,iy
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

void Game::getTileWorldPosition(int ix, int iy, float* ox, float* oy)
{
	float xpos = (ix - iy) * TILE_WIDTH / 2.0f + m_mapStartX;
	float ypos = (ix + iy) * TILE_HEIGHT / 3.0f - m_mapStartY;

	*ox = xpos;
	*oy = -ypos;
}

void Game::addBuilding(Building* build)
{
	m_buildings.push_back(build);
}

void Game::removeBuilding(Building* toRemove)
{
	for (BuildingList::iterator it = m_buildings.begin(); it != m_buildings.end(); ++it)
	{
		if (*it == toRemove)
		{
			delete *it;
			m_buildings.erase(it);
			break;
		}
	}
}

void Game::tileClicked(Tile* tile)
{
	// tmp
}