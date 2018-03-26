#pragma once

#include "Renderer2D.h"

// time in seconds between updating the power state
#define POWER_UPDATE_TIME 1
// time in seconds between updating buildings
#define HOUSE_UPDATE_TIME 2

// Forward declares
template <class T>
class DArray;

class Building;
class Game;

enum BuildingType;
enum ZoneType;

// typedef the DArray so it's shorter to type
typedef DArray<Building*> BuildingList;

class BuildingManager
{
public:
	//------------------------------------------------------------------------
	// Param:
	//			game:		pointer to our Game so we can access everything we 
	//                      need
	//			buildings:	pointer to the dynamic array of buildings
	//------------------------------------------------------------------------
	BuildingManager(Game* game, BuildingList* buildings);
	~BuildingManager();

	//------------------------------------------------------------------------
	// Called every frame when in building mode
	// Handles placement of buildings
	//------------------------------------------------------------------------
	void buildingMode();
	//------------------------------------------------------------------------
	// Called every frame when in building mode
	// Handles drawing of everything to do with building mode
	//
	// Param: 
	//			renderer: pointer to the renderer used to draw everything
	//------------------------------------------------------------------------
	void drawPlacement(aie::Renderer2D* renderer);
	//------------------------------------------------------------------------
	// Whether or not the ghost building is in a valid position to be placed
	//------------------------------------------------------------------------
	bool canPlaceBuilding();

	//------------------------------------------------------------------------
	// Gets the building on the tile at an index
	// Returns nullptr if there's no building at the position
	//
	// Param: 
	//			ix: the x index of the tile look for a building
	//			ix: the y index of the tile look for a building
	// Return: 
	//			pointer to the building at the position
	//------------------------------------------------------------------------
	Building* getBuildingAtIndex(int ix, int iy);
	//------------------------------------------------------------------------
	// Gets a pointer to the list of all buildings
	//
	// Return: 
	//			pointer to the dynamic array of buildings
	//------------------------------------------------------------------------
	BuildingList* getBuildings() { return m_buildings; }
	//------------------------------------------------------------------------
	// Deletes all buildings and clears the dynamic array
	//------------------------------------------------------------------------
	void clearBuildings();

	//------------------------------------------------------------------------
	// Updates all buildings and deals with updating things like power and 
	// zone population
	//
	// Param: 
	//			delta: time in seconds since the last update
	//------------------------------------------------------------------------
	void updateBuildings(float delta);
	//------------------------------------------------------------------------
	// Draws all buildings
	//
	// Param: 
	//			renderer: pointer to the renderer used to draw everything
	//------------------------------------------------------------------------
	void drawBuildings(aie::Renderer2D* renderer);
	//------------------------------------------------------------------------
	// Adds a building to the dynamic array
	// Should be the ONLY way buildings are added to the world
	//
	// Param: 
	//			build: the building to add to the array
	//			sort:  whether or not to sort the array after adding
	//------------------------------------------------------------------------
	void addBuilding(Building* build, bool sort = true);
	//------------------------------------------------------------------------
	// Used for when the player places a building
	// Differs from addBuilding because it checks for and deals with money
	//
	// Param: 
	//			build: the building to add to the array
	//			sort:  whether or not to sort the array after adding
	//------------------------------------------------------------------------
	void placeBuilding(Building* build, bool sort = true);
	//------------------------------------------------------------------------
	// Destroys and removes the specified building
	//
	// Param: 
	//			toRemove: the building to remove from the array
	//------------------------------------------------------------------------
	void removeBuilding(Building* toRemove);

	//------------------------------------------------------------------------
	// Sorts the array of buildings based on their depth from the camera
	// so they are drawn as expected in an isometric view
	//------------------------------------------------------------------------
	void sortBuildings();

	//------------------------------------------------------------------------
	// Gets the demand for the specified zone to be created
	//
	// Param: 
	//			zone: ZoneType enum of the zone to check the demand
	// Return: 
	//			a number representing demand of the zone
	//			anything under 1 means there is no demand for it
	//------------------------------------------------------------------------
	float getDemand(ZoneType zone);
	//------------------------------------------------------------------------
	// Gets how many buildings of a specific type exist in the world
	// Used in determining the demand for each zone
	//
	// Param: 
	//			type: BuildingType enum of which building to count
	// Return: 
	//			how many buildings of the type exist
	//------------------------------------------------------------------------
	int getBuildingCount(BuildingType type);

	//------------------------------------------------------------------------
	// Gets the BuildingType the player has selected to build as an integer
	//
	// Return: 
	//			which BuildingType is selected
	//------------------------------------------------------------------------
	inline int getSelectedBuilding() { return m_selectedBuilding; }
	//------------------------------------------------------------------------
	// Sets the BuildingType the player has selected
	//
	// Param: 
	//			id: BuildingType of the building to build
	//------------------------------------------------------------------------
	inline void setSelectedBuilding(int id) { m_selectedBuilding = id; }

	//------------------------------------------------------------------------
	// Creates a pointer to a new building
	// Used whenever making a building, basically a factory
	//
	// Param: 
	//			type:  BuildingType of the building to create
	//			xTile: tile-based x position of the building
	//			yTile: tile-based y position of the building
	//			ghost: whether or not we want a 'ghost' building which shows
	//                 a preview of where the building will be placed
	//------------------------------------------------------------------------
	Building* makeBuilding(BuildingType type, int xTile, int yTile,
		bool ghost = false);
private:
	Game*			m_game;
	BuildingList*	m_buildings;

	// type of building the player has selected to build
	int				m_selectedBuilding;
	// translucent building to show what the outcome will look like
	Building*		m_ghostBuilding;

	// variables used for clicking & dragging when building
	bool			m_dragging;
	int				m_dragStartX, m_dragStartY;
	int				m_dragPosX, m_dragPosY;
	bool			m_isDragHorizontal;

	// timers used to update power and zone population
	float			m_powerTimer;
	float			m_houseTimer;

	// variables to keep track of the number of buildings used in demand calcs
	int m_houseCount, m_shopCount, m_factoryCount;
	// functions called by getDemand for individual zone demands
	float getResidentialDemand();
	float getCommercialDemand();
	float getIndustrialDemand();
};