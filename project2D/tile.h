#pragma once

#include <Texture.h>

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

	aie::Texture* getTexture();
	void setTexture(aie::Texture* tex);

	inline ZoneType getZoneType() { return m_zoneType; }
	void setZoneType(ZoneType type);

protected:
	Game*			m_game;
	aie::Texture*	m_texture;

	ZoneType		m_zoneType;
private:
	unsigned int m_zoneTintColours[ZONETYPE_COUNT];
};