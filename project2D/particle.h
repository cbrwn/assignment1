#pragma once

#include "Renderer2D.h"

#include "vector2.h"

// Forward declares
class Game;

class Particle
{
public:
	//------------------------------------------------------------------------
	// Param: 
	//			game: pointer to our Game so we can access everything we need
	//			pos:  starting position of the particle
	//------------------------------------------------------------------------
	Particle(Game* game, Vector2& pos);
	virtual ~Particle() = default;

	//------------------------------------------------------------------------
	// Called every frame to deal with everything non-drawing related
	//
	// Param: 
	//			delta: time in seconds since the last frame
	//------------------------------------------------------------------------
	virtual void update(float delta) = 0;
	//------------------------------------------------------------------------
	// Also called every frame, dealing with everything drawing related
	//
	// Param: 
	//			renderer: a pointer to the Renderer2D we're using
	//------------------------------------------------------------------------
	virtual void draw(aie::Renderer2D* renderer);

	//------------------------------------------------------------------------
	// Gets the opacity of the particle
	// Used as the lifetime of the particle, so when this is 0 it should be 
	// deleted
	//
	// Return: 
	//			the opacity of the particle
	//------------------------------------------------------------------------
	float getOpacity() { return m_opacity; }
protected:
	Game* m_game;

	// transform values
	Vector2 m_pos, m_vel;
	float m_rotation;
	// opacity is also used as the lifetime of the particle
	float m_opacity;
	// texture to draw
	aie::Texture* m_texture;
};