#include <ctime>

#include <Texture.h>
#include <Font.h>
#include <Input.h>

#include "camera.h"
#include "game.h"
#include "imagemanager.h"
#include "tile.h"

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

	setBackgroundColour(0.12f, 0.63f, 1.0f);
	this->setShowCursor(false);

	m_2dRenderer = new aie::Renderer2D();
	m_imageManager = new ImageManager();
	m_camera = new Camera();

	//m_camera->setScale(0.5f);

	m_font = new aie::Font("./font/consolas.ttf", 32);

	m_mapStartX = 1200;
	m_mapStartY = 800;
	for (int y = 0; y < WORLD_HEIGHT; y++)
		for (int x = 0; x < WORLD_WIDTH; x++)
		{
			m_tiles[y][x] = new Tile(this, m_imageManager->getTexture("tiles/grass_flat"));
		}

	return true;
}

void Game::shutdown()
{

	delete m_font;
	delete m_2dRenderer;
	delete m_camera;

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

	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT))
	{
		float mx, my;
		this->getMouseXY(&mx, &my);
		int ix, iy;
		getTileAtPosition((int)mx, (int)my, &ix, &iy);
		Tile* mouseOver = m_tiles[iy][ix];
		if (mouseOver != nullptr)
		{
		}
	}

	m_camera->update(deltaTime);
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

			float xpos = (x - y) * TILE_WIDTH / 2.0f + m_mapStartX;
			float ypos = (x + y) * TILE_HEIGHT / 3.0f - m_mapStartY;

			// account for the difference in height in the texture
			// and keep the bottoms aligned
			float dify = TILE_HEIGHT - (float)thisTile->getTexture()->getHeight();

			thisTile->draw(m_2dRenderer, xpos, -ypos - dify / 2.0f);
			thisTile->setIndices(x, y);
		}
	}

	m_2dRenderer->drawCircle((float)mx, (float)my, 8);

	// done drawing sprites
	m_2dRenderer->end();

	// set camera back to 0,0 for ui stuff
	m_2dRenderer->setCameraPos(0, 0);

	// start drawing ui
	m_2dRenderer->begin();

	// output some text, uses the last used colour
	char fps[32];
	sprintf_s(fps, 32, "FPS: %i", getFPS());
	m_2dRenderer->drawText(m_font, fps, 2, 8);

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
