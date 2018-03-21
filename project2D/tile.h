#pragma once

#include <Texture.h>

class Building;
class Game;

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
	Tile(Game* game, aie::Texture* tex);

	virtual void draw(aie::Renderer2D* renderer, float dx, float dy,
		bool zoneTint);
	virtual void update();
	
	void setIndices(int x, int y);
	void getIndices(int* x, int* y);

	aie::Texture* getTexture();
	void setTexture(aie::Texture* tex);

	inline ZoneType getZoneType() { return m_zoneType; }
	void setZoneType(ZoneType type);

	inline bool hasPower() { return m_hasPower; }
	inline void setPower(bool p) { m_hasPower = p; }

	// each tile should keep track of the building that's on it
	inline Building* getBuilding() { return m_building; }
	inline void setBuilding(Building* b) { m_building = b; }

	bool isLiveable();
protected:
	Game*			m_game;
	aie::Texture*	m_texture;

	int m_xIndex, m_yIndex;

	ZoneType		m_zoneType;
	bool			m_hasPower;
	Building*		m_building;
private:
	static unsigned int m_zoneTintColours[ZONETYPE_COUNT];
};