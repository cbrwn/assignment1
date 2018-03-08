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
	Building* getBuildingAtIndex(int ix, int iy);
	BuildingList* getBuildings() { return m_buildings; }
	void clearBuildings();

	// general building functions
	void updateBuildings(float delta);
	void drawBuildings(aie::Renderer2D* renderer);
	void addBuilding(Building* build, bool sort = true);
	void removeBuilding(Building* toRemove);

	// quicksort functions!
	void sortBuildings();
	void sortBuildings(int min, int max);
	int partitionBuildings(int min, int max);

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

	float			m_updateTimer;
};

