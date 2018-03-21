#pragma once

#include <Renderer2D.h>
#include "vector2.h"

// when adding a new building type, don't forget to
//  add it to the factory in buildingmanager.cpp
//  and the building UI in uimanager.cpp
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

// enum to describe building styles
// whether a building is created individually,
//    in a line (click & drag) or in a rectangle (click & drag)
enum BuildStyle
{
	BUILDSTYLE_SINGLE = 0,
	BUILDSTYLE_LINE,
	BUILDSTYLE_RECT,
	BUILDSTYLE_FORBIDDEN // can't build!
};

class Game;
class Tile;

class Building
{
public:
	Building(Game* game, int x, int y);

	virtual void update(float delta);
	virtual void draw(aie::Renderer2D* renderer) = 0;

	// these will be called after the contructor/destructor 
	//   when added to the world
	virtual void created();
	virtual void destroyed();

	void drawEyeball(aie::Renderer2D* renderer, Vector2& pos, float rad);

	void setPosition(int x, int y);
	inline void getPosition(int* x, int* y) { *x = m_posX; *y = m_posY; }
	// getCenter is used for depth sorting
	void getCenter(int* x, int* y);
	inline void getSize(int* w, int* h) { *w = m_sizeX; *h = m_sizeY; }
	inline void setTexture(aie::Texture* tex) { m_texture = tex; }
	inline Vector2 getWorldPosition() { return m_worldPos; }

	// silly dropping stuff
	inline void setAltitude(float alt) { m_altitude = alt; }
	inline void setFallSpeed(float spd) { m_fallSpeed = spd; }

	bool updatePower();
	inline int getPowerSpread() { return m_powerSpreadRange; }
	inline int getPowerSearch() { return m_powerSearchRange; }

	inline BuildingType getType() { return m_type; }
	inline BuildStyle getBuildStyle() { return m_buildStyle; }

	inline int getPrice() { return m_price; }

	static char* buildingNames[BUILDINGTYPE_COUNT];
protected:
	Game*			m_game;

	int				m_posX, m_posY;
	int				m_sizeX, m_sizeY;
	// world position of the bulding from the tile number
	Vector2			m_worldPos;

	// used for dropping it into the world
	float			m_altitude;
	float			m_fallSpeed;
	bool			m_shakesCamera;

	int				m_powerSpreadRange; // how far this building shares power
	int				m_powerSearchRange; // how far this building looks for power
	bool			m_producesPower;
	bool			m_hasPower;

	// how long of a reach does affectTile have
	int				m_tileAffectRange;

	int				m_price;

	BuildingType	m_type;
	BuildStyle		m_buildStyle;
	aie::Texture*	m_texture;

	// these are used to more efficiently affect surrounding tiles' values
	//   when created/destroyed
	void affectOrUnaffectTiles(bool affect);
	virtual void affectTile(Tile* t);
	virtual void unaffectTile(Tile* t);
};