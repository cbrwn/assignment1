#pragma once

#include <Texture.h>

class Game;

class Tile
{
public:
	Tile(Game* game, aie::Texture* tex);

	virtual void draw(aie::Renderer2D* renderer, float dx, float dy);
	virtual void update();

	aie::Texture* getTexture();
	void setTexture(aie::Texture* tex);

protected:
	Game*			m_game;
	aie::Texture*	m_texture;
};