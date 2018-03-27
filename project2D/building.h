#pragma once

#include "Renderer2D.h"

#include "vector2.h"

// Enum used to indicate which building the object is when searching/creating
enum BuildingType
{
	BUILDINGTYPE_NONE = 0,
	BUILDINGTYPE_POWERPLANT,
	BUILDINGTYPE_POWERPOLE,
	BUILDINGTYPE_ROAD,
	BUILDINGTYPE_HOUSE,
	BUILDINGTYPE_SHOP,
	BUILDINGTYPE_FACTORY,

	BUILDINGTYPE_COUNT
};

// Enum describing how buildings should be created by the player
enum BuildStyle
{
	BUILDSTYLE_SINGLE = 0, // individually, one click = one building
	BUILDSTYLE_LINE, // click & drag to create a line of buildings
	BUILDSTYLE_RECT, // click & drag to create a rectangle of buildings
	BUILDSTYLE_FORBIDDEN // can't build! things like houses which are only
	                     // made automatically
};

// Forward declares
class Game;
class Tile;

class Building
{
public:
	//------------------------------------------------------------------------
	// Param:
	//			game: pointer to our Game so we can access everything we need
	//			x:    tile-based x position of the building
	//			y:    tile-based y position of the building
	//------------------------------------------------------------------------
	Building(Game* game, int x, int y);

	virtual ~Building() = default;

	//------------------------------------------------------------------------
	// Called every frame to deal with everything non-drawing related
	//
	// Param: 
	//			delta: time in seconds since the last frame
	//------------------------------------------------------------------------
	virtual void update(float delta);
	//------------------------------------------------------------------------
	// Also called every frame, dealing with everything drawing related
	//
	// Param: 
	//			renderer: a pointer to the Renderer2D we're using
	//------------------------------------------------------------------------
	virtual void draw(aie::Renderer2D* renderer) = 0;

	//------------------------------------------------------------------------
	// Called after the constructor happens, used to spread pollution and
	// other tile statuses
	//------------------------------------------------------------------------
	virtual void created();
	//------------------------------------------------------------------------
	// Called after the destructor, used to undo everything done in created()
	//------------------------------------------------------------------------
	virtual void destroyed();

	//------------------------------------------------------------------------
	// Draws an eyeball which looks towards the mouse
	//
	// Param: 
	//			renderer: a pointer to the Renderer2D we're using
	//			pos:      center point of the eye
	//			rad:      radius of the eye
	//------------------------------------------------------------------------
	void drawEyeball(aie::Renderer2D* renderer, Vector2& pos, float rad) const;

	//------------------------------------------------------------------------
	// Sets the position of the building, represented as the index of the tile
	// it's on in the 2D Tile array
	//
	// Param: 
	//			x: x index of the tile
	//			y: y index of the tile
	//------------------------------------------------------------------------

	void setPosition(int x, int y);
	//------------------------------------------------------------------------
	// Grabs the position, represented as the index of the tile
	// it's on in the 2D Tile array
	//
	// Param: 
	//			x: pointer to where the x position will be stored
	//			y: pointer to where the y position will be stored
	//------------------------------------------------------------------------
	void getPosition(int* x, int* y) const { *x = m_posX; *y = m_posY; }
	//------------------------------------------------------------------------
	// Grabs the indices of the center of the building
	// This is used in depth sorting, where we compare the center of the
	// building instead of the stored position (the bottom-right)
	//
	// Param: 
	//			x: pointer to where the x position of the center will be stored
	//			y: pointer to where the y position of the center will be stored
	//------------------------------------------------------------------------
	void getCenter(int* x, int* y) const;
	//------------------------------------------------------------------------
	// Gets the size (in tiles) of the building
	//
	// Param: 
	//			w: pointer to where the width will be stored
	//			h: pointer to where the height will be stored
	//------------------------------------------------------------------------
	void getSize(int* w, int* h) const { *w = m_sizeX; *h = m_sizeY; }
	//------------------------------------------------------------------------
	// Changes the texture which is used when drawing the building
	//
	// Param: 
	//			tex: the new texture
	//------------------------------------------------------------------------
	void setTexture(aie::Texture* tex) { m_texture = tex; }
	//------------------------------------------------------------------------
	// Gets the world position of the building, as opposed to the tile-based
	// positions stored in m_posX and m_posY
	//
	// Return: 
	//			Vector2 containing the world-based position of the building
	//------------------------------------------------------------------------
	Vector2 getWorldPosition() const { return m_worldPos; }

	//------------------------------------------------------------------------
	// Sets how far above the ground the building is
	// Used for dropping buildings into the world on creation
	//
	// Param: 
	//			alt: the desired altitude
	//------------------------------------------------------------------------
	void setAltitude(const float alt) { m_altitude = alt; }
	//------------------------------------------------------------------------
	// Sets how quickly the building falls to the ground
	// Used for dropping buildings into the world on creation
	//
	// Param: 
	//			spd: the desired speed
	//------------------------------------------------------------------------
	void setFallSpeed(const float spd) { m_fallSpeed = spd; }

	//------------------------------------------------------------------------
	// Powers this building if available, and spreads power if power is found
	//------------------------------------------------------------------------
	bool updatePower();
	//------------------------------------------------------------------------
	// Gets how many tiles to search when looking for power
	//
	// Return: 
	//			number of tiles from each side to search
	//------------------------------------------------------------------------
	int getPowerSearch() const { return m_powerSearchRange; }
	//------------------------------------------------------------------------
	// Gets how many tiles this building spreads power to if powered
	//
	// Return: 
	//			number of tiles from each side to spread
	//------------------------------------------------------------------------
	int getPowerSpread() const { return m_powerSpreadRange; }

	//------------------------------------------------------------------------
	// Gets the kind of building this is
	// Useful for checking a generic Building object for a specific type
	//
	// Return: 
	//			BuildingType enum of the building
	//------------------------------------------------------------------------
	BuildingType getType() const { return m_type; }
	//------------------------------------------------------------------------
	// Gets the method of placing the building in build mode
	//
	// Return: 
	//			BuildStyle enum of the building
	//------------------------------------------------------------------------
	BuildStyle getBuildStyle() const { return m_buildStyle; }

	//------------------------------------------------------------------------
	// Gets how much the building costs to build
	//
	// Return: 
	//			price of the building
	//------------------------------------------------------------------------
	int getPrice() const { return m_price; }

	// a static array of names to show in the UI
	static char* buildingNames[BUILDINGTYPE_COUNT];
protected:
	Game*			m_game;

	// positioning information
	int				m_posX, m_posY;
	int				m_sizeX, m_sizeY;
	Vector2			m_worldPos;

	// used for dropping it into the world
	float			m_altitude;
	float			m_fallSpeed;
	bool			m_shakesCamera;

	// how far this building shares power
	int				m_powerSpreadRange;
	// how far this building looks for power
	int				m_powerSearchRange;
	// whether or not this building creates power
	bool			m_producesPower;
	// whether or not this building is powered
	bool			m_hasPower;

	// how long of a reach does affectTile have
	int				m_tileAffectRange;

	// how much this building costs
	int				m_price;

	// which kind of building this is
	BuildingType	m_type;
	// how the player places this building
	BuildStyle		m_buildStyle;
	// the texture to draw
	aie::Texture*	m_texture;

	// these are used to more efficiently affect surrounding tiles' values
	//   when created/destroyed
	void affectOrUnaffectTiles(bool affect);
	virtual void affectTile(Tile* t);
	virtual void unaffectTile(Tile* t);
};