#pragma once

#include "Renderer2D.h"

#define POWER_UPDATE_TIME 1
#define HOUSE_UPDATE_TIME 2

template <class T>
class DArray;

class Building;
class Game;

enum BuildingType;
enum ZoneType;

// shorten the whole vector line
typedef DArray<Building*> BuildingList;

class BuildingManager
{
public:
	BuildingManager(Game* game, BuildingList* buildings);
	~BuildingManager();

	// building mode related functions
	void buildingMode();
	void drawPlacement(aie::Renderer2D* renderer);

	bool canPlaceBuilding();
	Building* getBuildingAtIndex(int ix, int iy);
	BuildingList* getBuildings() { return m_buildings; }
	void clearBuildings();

	// general building functions
	void updateBuildings(float delta);
	void drawBuildings(aie::Renderer2D* renderer);
	void addBuilding(Building* build, bool sort = true);
	void placeBuilding(Building* build, bool sort = true);
	void removeBuilding(Building* toRemove);

	// sorty stuff
	void sortBuildings();

	// demand for zones
	float getDemand(ZoneType zone);
	int getBuildingCount(BuildingType type);

	inline int getSelectedBuilding() { return m_selectedBuilding; }
	inline void setSelectedBuilding(int id) { m_selectedBuilding = id; }

	Building* makeBuilding(BuildingType type, int xTile, int yTile,
		bool ghost = false);
private:
	Game*			m_game;
	BuildingList*	m_buildings;

	int				m_selectedBuilding;
	// translucent building to show what the outcome will look like
	Building*		m_ghostBuilding;

	// dragging stuff
	bool			m_dragging;
	int				m_dragStartX, m_dragStartY;
	int				m_dragPosX, m_dragPosY;
	bool			m_isDragHorizontal;

	float			m_powerTimer;
	float			m_houseTimer;

	int m_houseCount, m_shopCount, m_factoryCount;
	float getResidentialDemand();
	float getCommercialDemand();
	float getIndustrialDemand();
};

