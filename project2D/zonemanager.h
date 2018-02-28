#pragma once

#include <Renderer2D.h>

enum ZoneType;

class Game;
class Tile;

class ZoneManager
{
public:
	ZoneManager(Game* game, Tile**** tiles);

	void zoneMode();

	void drawZones(aie::Renderer2D* renderer);

	inline void setSelectedType(ZoneType type) { m_selectedType = type; }
	inline ZoneType getSelectedType() { return m_selectedType; }
private:
	Game*		m_game;
	Tile****	m_tiles;

	// zone creation variables
	ZoneType m_selectedType;
	bool m_dragging;
	int m_dragStartX, m_dragStartY;
	int m_dragEndX, m_dragEndY;
};