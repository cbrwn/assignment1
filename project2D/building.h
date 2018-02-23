#pragma once

#include <Renderer2D.h>

// when adding a new building type, don't forget to
//  add it to the factory in buildingmanager.cpp
enum BuildingType
{
	POWERPLANT = 0,
	POWERPOLE,

	BUILDINGTYPECOUNT
};

// enum to describe building styles
// whether a building is created individually,
//    in a line (click & drag) or in a rectangle (click & drag)
enum BuildStyle
{
	SINGLE = 0,
	LINE,
	RECTANGLE
};

class Game;

class Building
{
public:
	Building(Game* game, int x, int y);

	virtual void update() = 0;
	virtual void draw(aie::Renderer2D* renderer) = 0;

	inline void setPosition(int x, int y) { m_posX = x; m_posY = y; }
	inline void getPosition(int* x, int* y) { *x = m_posX; *y = m_posY; }
	inline void getSize(int* w, int* h) { *w = m_sizeX; *h = m_sizeY; }

	inline BuildingType getType() { return m_type; }
protected:
	Game*			m_game;

	int				m_posX, m_posY;
	int				m_sizeX, m_sizeY;

	BuildingType	m_type;
	BuildStyle		m_buildStyle;
	aie::Texture*	m_texture;
};