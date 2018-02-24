#pragma once

#include <vector>
#include <Renderer2D.h>

#define BUILDING_UPDATE_TIME 1

class Building;
class Game;

enum BuildingType;

// shorten the whole vector line
typedef std::vector<Building*> BuildingList;

class BuildingManager
{
public:
	BuildingManager(Game* game, BuildingList* buildings);                      
	~BuildingManager();

	// building mode related functions
	void buildingMode();
	void drawPlacement(aie::Renderer2D* renderer);

	bool canPlaceBuilding();

	// general building functions
	void updateBuildings(float delta);
	void drawBuildings(aie::Renderer2D* renderer);

	inline BuildingList* getPowerPoles() { return &m_powerPoles; }

	inline int getSelectedBuilding() { return m_selectedBuilding; }
	inline void setSelectedBuilding(int id) { m_selectedBuilding = id; }

	Building* makeBuilding(BuildingType type, int xTile, int yTile, 
		bool ghost = false);
private:
	Game*			m_game;
	BuildingList*	m_buildings;

	BuildingList	m_powerPoles;

	int				m_selectedBuilding;
	// translucent building to show what the outcome will look like
	Building*		m_ghostBuilding;

	float			m_updateTimer;
};

