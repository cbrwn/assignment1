#pragma once

#include <string>

#include "Renderer2D.h"
#include "Application.h"

#include "vector2.h"

#define TILE_WIDTH 132
#define TILE_HEIGHT 99

#define WORLD_WIDTH 64
#define WORLD_HEIGHT 64

#define MAX_PARTICLES 4096

template <class T>
class DArray;

class Camera;
class Tile;
class Particle;
class Vector2;

class ImageManager;
class SaveManager;
class UiManager;

class Building;
class BuildingManager;
class RoadManager;
class TileManager;

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
	VIEWMODE_ZONE = 0b00000001,
	VIEWMODE_BUILDINGS = 0b00000010,
	VIEWMODE_POWER = 0b00000100,
	VIEWMODE_ROADS = 0b00001000
};

// typedef these so it's easier to type
typedef DArray<Building*> BuildingList;
typedef DArray<Particle*> ParticleList;

class Game : public aie::Application
{
public:
	Game();
	virtual ~Game();

	// we don't expect Game to be copied/moved
	Game(const Game& game)       = default;
	Game(Game&& game)            = default;
	Game& operator=(Game& game)  = default;
	Game& operator=(Game&& game) = default;

	// these shouldn't be public
	aie::Font* m_uiFont;
	aie::Font* m_uiFontLarge;

	bool startup() override;
	void shutdown() override;

	void update(float deltaTime) override;
	void draw() override;

	Vector2& getMapStart() { return m_mapStart; }

	// mouse-related functions
	Vector2 getMouseWorldPosition();
	Vector2 getMousePosition();
	bool	isMouseInGame();

	void drawTileRect(int left, int top, int right, int bottom);

	// money-related functions
	int  getMoney() { return m_money; }
	void setMoney(int money) { m_money = money; }
	void addMoney(int money) { m_money += money; }

	// place mode related functions
	PlaceMode getPlaceMode() { return m_placeMode; }
	void setPlaceMode(PlaceMode mode);

	// view mode related functions
	ViewMode getViewMode() { return m_viewMode; }
	void setViewMode(ViewMode mode) { m_viewMode = mode; }
	bool isViewModeEnabled(ViewMode mode) { return (m_viewMode & mode); };
	void toggleViewMode(ViewMode mode);

	// particle stuff
	void spawnSmokeParticle(Vector2& pos);
	void spawnPollutionParticle(Vector2& pos);
	void spawnTextParticle(Vector2& pos, std::string text);
	void doScreenShake(float amt);

	// getters for all of the managers
	ImageManager*		getImageManager() { return m_imageManager; }
	UiManager*			getUiManager() { return m_uiManager; }
	BuildingManager*	getBuildingManager() { return m_buildingManager; }
	RoadManager*		getRoadManager() { return m_roadManager; }
	SaveManager*		getSaveManager() { return m_saveManager; }
	TileManager*		getTileManager() { return m_tileManager; }

protected:
	aie::Renderer2D*	m_2dRenderer;
	Camera*				m_camera;

	// map/world-related stuff
	Vector2				m_mapStart;
	Tile***				m_tiles;
	BuildingList*		m_buildings;
	ParticleList*		m_particles;

	// icon shown in power viewmode
	aie::Texture*		m_powerIcon;

	// I'm so sorry
	ImageManager*		m_imageManager;
	UiManager*			m_uiManager;
	BuildingManager*	m_buildingManager;
	RoadManager*		m_roadManager;
	SaveManager*		m_saveManager;
	TileManager*		m_tileManager;

	// gameplay variables
	PlaceMode			m_placeMode;
	ViewMode			m_viewMode;
	int					m_money;
};