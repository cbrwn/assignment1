#pragma once

#include <Renderer2D.h>

enum ZoneType;

class Game;
class Tile;

class TileManager
{
public:
	TileManager(Game* game, Tile**** tiles);

	void updateZoneEditing();

	void drawZoneSelection(aie::Renderer2D* renderer);

	inline Tile* getTile(int x, int y) { return (*m_tiles)[y][x]; }
	void	getTileAtMousePosition(int *ix, int *iy);
	Tile*	getTileAtPosition(Vector2& pos);
	void	getTileAtPosition(Vector2& pos, int* ix, int* iy);
	Vector2 getTileWorldPosition(int ix, int iy);
	void	clearTilePower();

	inline void setSelectedType(ZoneType type) { m_selectedType = type; }
	inline ZoneType getSelectedType() { return m_selectedType; }
private:
	Game * m_game;
	Tile****	m_tiles;

	// zone creation variables
	ZoneType m_selectedType;
	bool m_dragging;
	int m_dragStartX, m_dragStartY;
	int m_dragEndX, m_dragEndY;
};