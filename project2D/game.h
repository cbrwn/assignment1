#pragma once

#include <vector>
#include "Application.h"
#include "Renderer2D.h"
#include "imagemanager.h"

#define TILE_WIDTH 132
#define TILE_HEIGHT 99

#define WORLD_WIDTH 64
#define WORLD_HEIGHT 64

class Camera;
class Tile;

class ImageManager;
class UiManager;

class Building;
class BuildingManager;
class ZoneManager;

// place modes are the modes the player can switch between to place
//   different kinds of things
// building mode lets you place buildings and zone mode lets you set zones
//   (residential, commercial, industrial)
enum PlaceMode
{
	PLACEMODE_NONE = 0,
	PLACEMODE_BUILDING,
	PLACEMODE_ZONE
};

// view modes are modes which can be enabled/disabled to see certain things,
//  like toggling the ability to see zones
enum ViewMode
{
	VIEWMODE_ZONE = 0b00000001
};

// typedef building vector so it's shorter to type
typedef std::vector<Building*> BuildingList;

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

	// mouse-related functions
	void getMouseWorldPosition(float* x, float* y);
	bool isMouseInGame();

	// tile-related functions
	Tile* getTile(int x, int y) { return m_tiles[y][x]; }
	Tile* getTileAtPosition(float x, float y);
	void  getTileAtPosition(float px, float py, int *ix, int *iy);
	void  getTileAtMousePosition(int *ix, int *iy);
	void  getTileWorldPosition(int ix, int iy, float* ox, float* oy);

	// building-related functions
	// (could/should move to BuildingManager)
	void addBuilding(Building* build);
	void removeBuilding(Building* toRemove);
	void sortBuildings();

	// money-related functions
	inline int  getMoney() { return m_money; }
	inline void setMoney(int money) { m_money = money; }
	inline void addMoney(int money) { m_money += money; }

	// place mode related functions
	inline PlaceMode getPlaceMode() { return m_placeMode; }
	void setPlaceMode(PlaceMode mode);

	// getters for all of the managers
	inline ImageManager*	getImageManager() { return m_imageManager; }
	inline UiManager*		getUiManager() { return m_uiManager; }
	inline BuildingManager* getBuildingManager() { return m_buildingManager; }
	inline ZoneManager*		getZoneManager() { return m_zoneManager; }

protected:
	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;
	Camera*				m_camera;

	float				m_mapStartX, m_mapStartY;
	Tile***				m_tiles;
	BuildingList m_buildings;

	ImageManager*		m_imageManager;
	UiManager*			m_uiManager;
	BuildingManager*	m_buildingManager;
	ZoneManager*		m_zoneManager;

	// gameplay variables
	PlaceMode m_placeMode;
	ViewMode m_viewMode;
	int m_money;
};