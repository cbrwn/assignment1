#pragma once

#include "Texture.h"
#include "Renderer2D.h"

// Forward declares
class Building;
class Game;

// Enum for which zone a tile is
enum ZoneType
{
	ZONETYPE_NONE = 0,
	ZONETYPE_RESIDENTIAL,
	ZONETYPE_COMMERCIAL,
	ZONETYPE_INDUSTRIAL,

	ZONETYPE_COUNT // the total number of zones
};

class Tile
{
public:
	//------------------------------------------------------------------------
	// Param:
	//			game: pointer to our Game so we can access everything we need
	//			tex:  texture for drawing
	//------------------------------------------------------------------------
	Tile(Game* game, aie::Texture* tex);

	virtual ~Tile() = default;

	//------------------------------------------------------------------------
	// Called every frame, draws the texture
	//
	// Param: 
	//			renderer: a pointer to the Renderer2D used to draw everything
	//			dx:       x position to draw the tile
	//			dy:       y position to draw the tile
	//			zoneTint: whether the tile should have the colour of its zone
	//------------------------------------------------------------------------
	virtual void draw(aie::Renderer2D* renderer, float dx, float dy, 
		bool zoneTint);

	//------------------------------------------------------------------------
	// Gets the indeices of the tile in the 2D array
	//
	// Param: 
	//			x: pointer to where the x index should be stored
	//			y: pointer to where the y index should be stored
	//------------------------------------------------------------------------
	void getIndices(int* x, int* y) const;
	//------------------------------------------------------------------------
	// Set the indices of the tile in the 2D array
	// Just an easy way to check where in the array the tile is
	//
	// Param: 
	//			x: x index of the 2D array
	//			y: y index of the 2D array
	//------------------------------------------------------------------------
	void setIndices(int x, int y);

	//------------------------------------------------------------------------
	// Gets the texture of the tile
	//
	// Return: 
	//			the texture to be drawn
	//------------------------------------------------------------------------
	aie::Texture* getTexture() const { return m_texture; }
	//------------------------------------------------------------------------
	// Sets the texture to draw
	//
	// Param: 
	//			tex: new texture
	//------------------------------------------------------------------------
	void setTexture(aie::Texture* tex) { m_texture = tex; }
	
	//------------------------------------------------------------------------
	// Gets the type of zone this tile is
	//
	// Return: 
	//			ZoneType enum of the zone type
	//------------------------------------------------------------------------
	ZoneType getZoneType() const { return m_zoneType; }
	//------------------------------------------------------------------------
	// Sets the type of zone of the tile
	//
	// Param: 
	//			type: the new ZoneType of the tile
	//------------------------------------------------------------------------
	void setZoneType(const ZoneType type) { m_zoneType = type; }

	//------------------------------------------------------------------------
	// Gets whether or not the tile is powered
	//
	// Return: 
	//			whether or not the tile has power
	//------------------------------------------------------------------------
	bool hasPower() const { return m_hasPower; }
	//------------------------------------------------------------------------
	// Changes the power state of the tile
	//
	// Param: 
	//			p: new power state
	//------------------------------------------------------------------------
	void setPower(const bool p) { m_hasPower = p; }

	//------------------------------------------------------------------------
	// Gets the pollution value of the tile
	// Pollution is caused by nearby factories
	//
	// Return: 
	//			pollution value
	//------------------------------------------------------------------------
	int getPollution() const { return m_pollution; }
	//------------------------------------------------------------------------
	// Sets the pollution value
	//
	// Param: 
	//			p: new pollution value
	//------------------------------------------------------------------------
	void setPollution(const int p) { m_pollution = p; }
	//------------------------------------------------------------------------
	// Adds an amount to the pollution value
	//
	// Param: 
	//			p: number to add to the pollution value
	//------------------------------------------------------------------------
	void addPollution(const int p) { m_pollution += p; }

	//------------------------------------------------------------------------
	// Gets the building on this tile
	//
	// Return: 
	//			a pointer to the building on the tile, or nullptr if none
	//------------------------------------------------------------------------
	Building* getBuilding() const { return m_building; }
	//------------------------------------------------------------------------
	// Sets the building on this tile
	//
	// Param: 
	//			b: pointer to the building on the tile
	//------------------------------------------------------------------------
	void setBuilding(Building* b) { m_building = b; }

	//------------------------------------------------------------------------
	// Gets whether or not this tile is able to spawn buildings
	// Based on whether or not it has power, has a zone and has a close road
	//
	// Return: 
	//			whether or not the tile is suitable for buildings
	//------------------------------------------------------------------------
	bool isLiveable() const;
protected:
	Game*			m_game;
	aie::Texture*	m_texture;

	// array indices
	int m_xIndex, m_yIndex;

	// general tile info
	Building*		m_building; // building on this tile
	ZoneType		m_zoneType;
	bool			m_hasPower;
	int				m_pollution;
private:
	// colours to tint the tile when zone tinting is enabled
	static unsigned int m_zoneTintColours[ZONETYPE_COUNT];
};