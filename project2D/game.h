#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "imagemanager.h"

#define TILE_WIDTH 132
#define TILE_HEIGHT 99

#define WORLD_WIDTH 16
#define WORLD_HEIGHT 16

class Camera;
class TextPopup;
class Tile;

class ImageManager;
class PeopleManager;
class UiManager;

enum TileType;

class Game : public aie::Application
{
public:
	Game();
	virtual ~Game();

	aie::Font* m_uiFont;
	aie::Font* m_uiFontLarge;

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void updateTiles();

	void getMouseXY(float* x, float* y);
	bool isMouseInGame();

	Tile* getTile(int x, int y) { return m_tiles[y][x]; }
	Tile* getTileAtPosition(int x, int y);
	void  getTileAtPosition(int px, int py, int *ix, int *iy);
	void  getTileWorldPosition(int ix, int iy, float* ox, float* oy);

	int  getMoney() { return m_money; }
	void setMoney(int money) { m_money = money; }
	void addMoney(int money) { m_money += money; }

	char* getTileName(TileType type);

	int  getPlaceMode() { return m_placeMode; }
	void setPlaceMode(int mode) { m_placeMode = mode; }

	ImageManager*	getImageManager() { return m_imageManager; }
	PeopleManager*	getPeopleManager() { return m_peopleManager; }
	UiManager*		getUimanager() { return m_uiManager; }

protected:
	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;
	Camera*				m_camera;

	Tile*				m_tiles[WORLD_HEIGHT][WORLD_WIDTH];
	ImageManager*		m_imageManager;
	PeopleManager*		m_peopleManager;
	UiManager*			m_uiManager;

	float				m_mapStartX, m_mapStartY;

	char*				m_tileNames[64];
	int					m_placeMode;

	// actual gameplay variables
	int m_money;

	void tileClicked(Tile* tile);
};