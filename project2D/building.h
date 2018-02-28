#pragma once

#include <Renderer2D.h>

// when adding a new building type, don't forget to
//  add it to the factory in buildingmanager.cpp
enum BuildingType
{
	BUILDINGTYPE_POWERPLANT = 0,
	BUILDINGTYPE_POWERPOLE,

	BUILDINGTYPE_COUNT
};

// enum to describe building styles
// whether a building is created individually,
//    in a line (click & drag) or in a rectangle (click & drag)
enum BuildStyle
{
	BUILDSTYLE_SINGLE = 0,
	BUILDSTYLE_LINE,
	BUILDSTYLE_RECT
};

class Game;

class Building
{
public:
	Building(Game* game, int x, int y);

	virtual void update();
	virtual void draw(aie::Renderer2D* renderer) = 0;

	void setPosition(int x, int y);
	inline void getPosition(int* x, int* y) { *x = m_posX; *y = m_posY; }
	inline void getSize(int* w, int* h) { *w = m_sizeX; *h = m_sizeY; }

	inline bool hasPower() { return m_hasPower; }
	// give this building power - used by power plants to give poles power
	inline void givePower() { m_hasPower = true; }

	inline BuildingType getType() { return m_type; }
	inline BuildStyle getBuildStyle() { return m_buildStyle; }
protected:
	Game*			m_game;

	int				m_posX, m_posY;
	int				m_sizeX, m_sizeY;
	// world position of the bulding from the tile number
	float			m_worldX, m_worldY;

	bool			m_hasPower;

	BuildingType	m_type;
	BuildStyle		m_buildStyle;
	aie::Texture*	m_texture;
};