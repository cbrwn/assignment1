#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "imagemanager.h"

#define TILE_WIDTH 132
#define TILE_HEIGHT 99

#define WORLD_WIDTH 32
#define WORLD_HEIGHT 32

class Camera;
class ImageManager;
class PeopleManager;
class TextPopup;
class Tile;

class Game : public aie::Application
{
public:
	Game();
	virtual ~Game();

	Tile*			m_tiles[WORLD_HEIGHT][WORLD_WIDTH];
	ImageManager*	m_imageManager;
	PeopleManager*  m_peopleManager;

	static Game* getInstance() { return m_instance; };

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void updateTiles();

	void getMouseXY(float* x, float* y);

	Tile* getTileAtPosition(int x, int y);
	void getTileAtPosition(int px, int py, int *ix, int *iy);
	void getTileWorldPosition(int ix, int iy, float* ox, float* oy);

	int getMoney() { return m_money; }
	void setMoney(int money) { m_money = money; }
	void addMoney(int money) { m_money += money; }

	void addTextPopup(char* text, float startX, float startY);
protected:
	static Game*		m_instance;

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;
	aie::Font*			m_uiFont;
	aie::Font*			m_uiFontLarge;
	Camera*				m_camera;

	float				m_mapStartX, m_mapStartY;
	int					m_placeMode;

	char*				m_tileNames[64];

	#define MAX_POPUPS 32
	TextPopup*			m_popups[MAX_POPUPS];

	// actual gameplay variables
	int m_money;

	void tileClicked(Tile* tile);
};