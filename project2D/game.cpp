#include <ctime>
#include <functional>

#include <Texture.h>
#include <Font.h>
#include <Input.h>

#include "camera.h"
#include "game.h"
#include "peoplemanager.h"
#include "textpopup.h"

#include "tile.h"
#include "tile_house.h"
#include "tile_shop.h"

Game* Game::m_instance = nullptr;

Game::Game()
{
	m_instance = this;
}

Game::~Game()
{

}

bool Game::startup()
{
	srand((unsigned int)time(NULL));

	// set up tile names
	m_tileNames[GRASS] = "Grass";
	m_tileNames[HOUSE] = "House";
	m_tileNames[SHOP] = "Shop";

	setBackgroundColour(0.12f, 0.63f, 1.0f);
	//this->setShowCursor(false);

	m_2dRenderer = new aie::Renderer2D();
	m_imageManager = new ImageManager();
	m_camera = new Camera();

	m_camera->setScale(1.3f);

	m_font = new aie::Font("./font/consolas.ttf", 32);
	m_uiFont = new aie::Font("./font/roboto.ttf", 16);
	m_uiFontLarge = new aie::Font("./font/roboto.ttf", 24);

	m_peopleManager = new PeopleManager(this);

	// initialize all popups to nullptr
	for (int i = 0; i < MAX_POPUPS; i++)
	{
		m_popups[i] = nullptr;
	}

	m_placeMode = 0;
	m_mapStartX = 1200;
	m_mapStartY = 800;
	for (int y = 0; y < WORLD_HEIGHT; y++)
		for (int x = 0; x < WORLD_WIDTH; x++)
		{
			m_tiles[y][x] = new Tile(this, m_imageManager->getTexture("tiles/grass_flat"));
		}

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
	delete m_peopleManager;

	for (int y = 0; y < WORLD_HEIGHT; y++)
		for (int x = 0; x < WORLD_WIDTH; x++)
			if (m_tiles[y][x] != nullptr)
				delete m_tiles[y][x];

	// delete all the popups
	for (int i = 0; i < MAX_POPUPS; i++)
	{
		if (m_popups[i] != nullptr)
			delete m_popups[i];
	}
}

void Game::update(float deltaTime)
{
	// input example
	aie::Input* input = aie::Input::getInstance();

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT))
	{
		float mx, my;
		this->getMouseXY(&mx, &my);
		Tile* mouseOver = getTileAtPosition((int)mx, (int)my);
		if (mouseOver != nullptr)
		{
			tileClicked(mouseOver);
		}
	}

	const int highestPlaceMode = 2;
	if (input->wasKeyPressed(aie::INPUT_KEY_COMMA))
	{
		m_placeMode--;
		if (m_placeMode < 0)
			m_placeMode = highestPlaceMode;
	}
	if (input->wasKeyPressed(aie::INPUT_KEY_PERIOD))
	{
		m_placeMode++;
		if (m_placeMode > highestPlaceMode)
			m_placeMode = 0;
	}

	m_peopleManager->update(deltaTime);

	m_camera->update(deltaTime);

	// update money popups
	// todo: make this a general 'effect' thing
	for (int i = 0; i < MAX_POPUPS; i++)
	{
		TextPopup* thisPopup = m_popups[i];
		if (thisPopup != nullptr)
		{
			thisPopup->update(deltaTime);
			if (thisPopup->shouldDie())
			{
				delete thisPopup;
				m_popups[i] = nullptr;
			}
		}
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
	float mx, my;
	this->getMouseXY(&mx, &my);
	Tile* mouseOver = this->getTileAtPosition((int)mx, (int)my);
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

			float xpos;// = (x - y) * TILE_WIDTH / 2.0f + m_mapStartX;
			float ypos;// = (x + y) * TILE_HEIGHT / 3.0f - m_mapStartY;

			getTileWorldPosition(x, y, &xpos, &ypos);

			// account for the difference in height in the texture
			// and keep the bottoms aligned
			float dify = TILE_HEIGHT - (float)thisTile->getTexture()->getHeight();

			thisTile->draw(m_2dRenderer, xpos, -ypos - dify / 2.0f);

			// make sure the tile knows its place in the array
			thisTile->setIndices(x, y);
		}
	}

	// draw money popups
	for (int i = 0; i < MAX_POPUPS; i++)
	{
		TextPopup* thisPopup = m_popups[i];
		if (thisPopup != nullptr)
			thisPopup->draw(m_2dRenderer);
	}

	// done drawing sprites
	m_2dRenderer->end();

	// set camera back to 0,0 for ui stuff
	m_2dRenderer->setCameraPos(0, 0);
	m_2dRenderer->setCameraScale(1.0f);

	// start drawing ui
	m_2dRenderer->begin();

	// show screen's mouse position (as opposed to the world mouse position)
	int smx, smy;
	aie::Input::getInstance()->getMouseXY(&smx, &smy);
	m_2dRenderer->drawCircle((float)smx, (float)smy, 8);

	// draw mouseover information
	if (mouseOver != nullptr)
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
		}
	}

	//===========
	// hud stuff
	//===========

	// show population
	char pop[64];
	sprintf_s(pop, 64, "Unemployment: %d / %d", m_peopleManager->getUnemployed(),
		m_peopleManager->getTotalPopulation());
	m_2dRenderer->setRenderColour(1, 1, 1);
	m_2dRenderer->drawText(m_uiFontLarge, pop, 2, 720 - 18);

	// temp - show which tile we're placing
	char plc[32];
	sprintf_s(plc, 32, "Placing %s tile", m_tileNames[m_placeMode]);
	m_2dRenderer->drawText(m_uiFontLarge, plc, 2, 720 - (18 * 2));

	// show money
	char mny[32];
	sprintf_s(mny, 32, "$%d", m_money);
	float moneyWidth = m_uiFontLarge->getStringWidth(mny);
	m_2dRenderer->setRenderColour(0, 0.4f, 0);
	m_2dRenderer->drawText(m_uiFontLarge, mny, 1280 - moneyWidth - 2, 720 - 18);

	// show fps
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", getFPS());
	m_2dRenderer->setRenderColour(0, 0, 0);
	m_2dRenderer->drawText(m_uiFont, fps, 2, 6);

	// done drawing sprites
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

void Game::getMouseXY(float* x, float* y)
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

// gets the tile at world position x,y 
Tile * Game::getTileAtPosition(int x, int y)
{
	int ix, iy;
	this->getTileAtPosition(x, y, &ix, &iy);

	if (ix < 0 || iy < 0)
		return nullptr;
	return m_tiles[iy][ix];
}

// gets the array indices of the tile at world position px,py and puts them into ix,iy
void Game::getTileAtPosition(int px, int py, int* ix, int* iy)
{
	// get shorter names for the measurements we use to offset the tiles
	const float tw = TILE_WIDTH / 2.0f;
	const float th = TILE_HEIGHT / 3.0f;

	// make float copies of px and py instead of passing them as floats because I'm dumb
	float fpx = (float)px;
	float fpy = (float)py;

	// idk dude i'm too tired to write comments
	fpy -= th;

	// invert y since y=0 is at the bottom
	fpy *= -1;

	fpx -= m_mapStartX;
	fpy += m_mapStartY;

	int resultX = (int)(((fpx) / tw + fpy / th) / 2);
	int resultY = (int)((fpy / th - (fpx / tw)) / 2);

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
	*oy = ypos;
}

void Game::tileClicked(Tile* tile)
{
	int ix, iy;
	tile->getIndices(&ix, &iy);

	bool facing = false;
	if (rand() % 2 == 0)
		facing = true;

	// check which tile we want to place
	Tile* newTile;
	switch (m_placeMode)
	{
	case TileType::GRASS:
		newTile = new Tile(this, m_imageManager->getTexture("tiles/grass_flat"));
		break;
	case TileType::HOUSE:
		newTile = new TileHouse(this, facing);
		break;
	case TileType::SHOP:
		newTile = new TileShop(this, facing);
		break;
	default:
		printf("Unknown place mode: %d\n", m_placeMode);
		return;
	}
	delete tile; // free up where the old tile was
	// and replace it
	m_tiles[iy][ix] = newTile;
}

void Game::addTextPopup(char* text, float startX, float startY)
{
	// CONTINUE HERE

	// get the first empty element in popup array
	int freeIndex = 0;
	while (freeIndex < MAX_POPUPS && m_popups[freeIndex] != nullptr)
	{ freeIndex++; }

	TextPopup* newPopup = new TextPopup(text, startX, startY, 1.0f, 30.0f, m_uiFont);
	m_popups[freeIndex] = newPopup;
}
