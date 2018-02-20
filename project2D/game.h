#pragma once

#include "Application.h"
#include "Renderer2D.h"

#define TILE_WIDTH 132
#define TILE_HEIGHT 99

#define WORLD_WIDTH 32
#define WORLD_HEIGHT 32

class Camera;
class ImageManager;
class Tile;

class Game : public aie::Application
{
public:

	Game();
	virtual ~Game();

	Tile*			m_tiles[WORLD_HEIGHT][WORLD_WIDTH];
	ImageManager*	m_imageManager;

	static Game* getInstance() { return m_instance; };

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void updateTiles();

	void getMouseXY(float* x, float* y);

	Tile* getTileAtPosition(int x, int y);
	void getTileAtPosition(int px, int py, int *ix, int *iy);

protected:
	static Game*		m_instance;

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;
	Camera*				m_camera;

	float				m_mapStartX, m_mapStartY;
};