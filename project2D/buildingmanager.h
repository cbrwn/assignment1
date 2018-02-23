#pragma once

#include <vector>
#include <Renderer2D.h>

class Building;
class Game;

enum BuildingType;

typedef std::vector<Building*> BuildingList;

class BuildingManager
{
public:
	BuildingManager(Game* game, BuildingList* buildings);
	~BuildingManager();

	void buildingMode();
	void draw(aie::Renderer2D* renderer);

	void updateBuildings(float delta);
	void drawBuildings(aie::Renderer2D* renderer);

	bool canPlaceBuilding();

	inline int getSelectedBuilding() { return m_selectedBuilding; }
	inline void setSelectedBuilding(int id) { m_selectedBuilding = id; }

	Building* makeBuilding(BuildingType type, int xTile, int yTile);
private:
	Game* m_game;
	BuildingList* m_buildings;

	int m_selectedBuilding;
	// translucent building to show what the outcome will look like
	Building* m_ghostBuilding;
};

