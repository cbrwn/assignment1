#pragma once

#include <Texture.h>

class Game;

enum TileType
{
	GRASS = 0,
	HOUSE,
	SHOP
};

class Tile
{
public:
	Tile(Game* game, aie::Texture* tex);

	virtual void draw(aie::Renderer2D* renderer, float dx, float dy);
	virtual void update();

	virtual char* getMouseoverText();

	aie::Texture* getTexture();
	void setTexture(aie::Texture* tex);

	TileType getType();

	void setIndices(int ix, int iy);
	void getIndices(int* ix, int* iy);

protected:
	// indices in the 2d tile array
	int m_ix, m_iy;

	Game*			m_game;
	aie::Texture*	m_texture;
	TileType		m_tileType;
};