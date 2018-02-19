#pragma once

#include "Application.h"
#include "Renderer2D.h"

#define TILE_WIDTH 132
#define TILE_HEIGHT 99

#define WORLD_WIDTH 128
#define WORLD_HEIGHT 128

class ImageManager;
class Tile;

class Game : public aie::Application
{
public:

	Game();
	virtual ~Game();

	static Game* getInstance() { return m_instance; };

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void getMouseXY(float* x, float* y);

	Tile* getTileAtPosition(int x, int y);
	void getTileAtPosition(int px, int py, int *ix, int *iy);

protected:
	static Game*		m_instance;

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;
	ImageManager*		m_imageManager;

	Tile*				m_tiles[WORLD_HEIGHT][WORLD_WIDTH];

	float m_cameraX, m_cameraY;
	float m_timer;
	float m_mapStartX, m_mapStartY;
};