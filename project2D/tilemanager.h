#pragma once

#include "Renderer2D.h"

#include "vector2.h"

enum ZoneType;

class Game;
class Tile;

class TileManager
{
public:
	//------------------------------------------------------------------------
	// Param:
	//			game:  pointer to our Game so we can access everything we need
	//			tiles: pointer to the 2D array of Tile pointers
	//------------------------------------------------------------------------
	TileManager(Game* game, Tile**** tiles);

	//------------------------------------------------------------------------
	// Called every frame when in zone editing mode
	// Handles setting zones
	//------------------------------------------------------------------------
	void updateZoneEditing();
	//------------------------------------------------------------------------
	// Called every frame when in building mode
	// Draws the rectangle showing where the player is selecting
	//
	// Param: 
	//			renderer: pointer to the renderer used to draw everything
	//------------------------------------------------------------------------
	void drawZoneSelection(aie::Renderer2D* renderer);

	//------------------------------------------------------------------------
	// Gets a pointer to the tile at a specified index in the 2D array
	// Simply indexes the array, so the input should always be checked 
	// beforehand
	//
	// Param: 
	//			x: x index of the desired tile
	//			y: y index of the desired tile
	// Return: 
	//			the tile at that location
	//------------------------------------------------------------------------
	inline Tile* getTile(int x, int y) { return (*m_tiles)[y][x]; }
	//------------------------------------------------------------------------
	// Grabs the tile index which is under the mouse's world position
	// Indices will be -1 if no tile is found
	//
	// Param: 
	//			ix: pointer to where the resulting x index should be stored 
	//			iy: pointer to where the resulting y index should be stored 
	//------------------------------------------------------------------------
	void	getTileAtMousePosition(int *ix, int *iy);
	//------------------------------------------------------------------------
	// Gets a pointer to the tile at a specified world position
	//
	// Param: 
	//			pos: Vector2 representing the position to search for
	// Return: 
	//			the tile at the position OR nullptr if no tile exists there
	//------------------------------------------------------------------------
	Tile*	getTileAtPosition(Vector2& pos);
	//------------------------------------------------------------------------
	// Gets the index to the tile at a specified world position
	//
	// Param: 
	//			pos: Vector2 representing the position to search for
	//			ix:  pointer to where the resulting x index should be stored 
	//			iy:  pointer to where the resulting y index should be stored 
	//------------------------------------------------------------------------
	void	getTileAtPosition(Vector2& pos, int* ix, int* iy);
	//------------------------------------------------------------------------
	// Gets the world position of the tile at the specified index
	//
	// Param: 
	//			ix: x index of tile to get the position of
	//			iy: y index of tile to get the position of
	// Return: 
	//			Vector2 representing the position of the tile at the index
	//------------------------------------------------------------------------
	Vector2 getTileWorldPosition(int ix, int iy);
	//------------------------------------------------------------------------
	// Checks if an index is within the world bounds
	// Useful for checking the indices before using getTile
	//
	// Param: 
	//			x: x index to check
	//			y: y index to check
	// Return: 
	//			whether or not the indices are within the bounds of the world
	//------------------------------------------------------------------------
	bool	isIndexInBounds(int x, int y);
	//------------------------------------------------------------------------
	// Sets all tiles to have no power
	// Used when updating tile power
	//------------------------------------------------------------------------
	void	clearTilePower();

	//------------------------------------------------------------------------
	// Deletes all tiles and creates a new array of them
	//
	// Param: 
	//			width:  the width of the new array
	//			height: the height of the new array
	//------------------------------------------------------------------------
	void clearTiles(int width, int height);

	//------------------------------------------------------------------------
	// Sets the zone type which will be placed by the player
	//
	// Param: 
	//			type: zone type selected by the player
	//------------------------------------------------------------------------
	inline void setSelectedType(ZoneType type) { m_selectedType = type; }
	//------------------------------------------------------------------------
	// boopydoopy
	//
	// Param: 
	//			boop: doop
	// Return: 
	//			a boop that doops
	//------------------------------------------------------------------------
	inline ZoneType getSelectedType() { return m_selectedType; }
private:
	Game* m_game;
	// a pointer to the 2D array of Tile pointers
	Tile****	m_tiles;

	// zone creation variables
	ZoneType m_selectedType;
	// dragging variables
	bool m_dragging;
	int m_dragStartX, m_dragStartY;
	int m_dragEndX, m_dragEndY;
};