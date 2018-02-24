////////////////////////////////////////////
// Camera
// Controls everything to do with the view
//   of the game - letting the player move
//   it and smoothly moving it
////////////////////////////////////////////

#include <Input.h>

#include "camera.h"
#include "game.h"

Camera::Camera(Game* game) : m_game(game)
{
	// default values
	m_targetX = 0; m_targetY = 0;
	m_actualX = 0; m_actualY = 0;
	m_targetScale = 1.0f; m_actualScale = 1.0f;

	m_smoothSpeed = 10.0f;
	m_zoomSpeed = 10.0f;
	m_arrowSpeed = 1000.0f;

	m_dragging = false;
	m_lastScroll = 0.0f;
}

void Camera::update(float delta)
{
	// arrow key control
	aie::Input* input = aie::Input::getInstance();
	if (input->isKeyDown(aie::INPUT_KEY_LEFT))
		m_targetX -= m_arrowSpeed * delta;
	if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
		m_targetX += m_arrowSpeed * delta;
	if (input->isKeyDown(aie::INPUT_KEY_UP))
		m_targetY += m_arrowSpeed * delta;
	if (input->isKeyDown(aie::INPUT_KEY_DOWN))
		m_targetY -= m_arrowSpeed * delta;

	// dragging control
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT))
	{
		int mx, my;
		input->getMouseXY(&mx, &my);
		float wx = (float)mx; float wy = (float)my;
		screenToWorld(&wx, &wy);

		if (!m_dragging && m_game->isMouseInGame() &&
			input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_RIGHT))
		{
			// start dragging if mouse is down
			m_dragging = true;
			m_dragStartMX = wx; m_dragStartMY = wy;
			m_dragStartCX = m_actualX; m_dragStartCY = m_actualY;
		}
		if(m_dragging)
		{
			// move the camera with the mouse
			setX(m_dragStartCX + (m_dragStartMX - wx)*4.0f);
			setY(m_dragStartCY + (m_dragStartMY - wy)*4.0f);
		}
	}
	else
	{
		// stop dragging when mouse button is released
		m_dragging = false;
	}

	// mousewheel scroll
	double scrollAmt = input->getMouseScroll();
	double dif = scrollAmt - m_lastScroll;
	m_targetScale -= (float)dif * 0.1f;
	m_lastScroll = scrollAmt;

	if (m_targetScale < 0.2f)
		m_targetScale = 0.2f;

	// smoothly moves towards target position
	m_actualX -= (m_actualX - m_targetX) * m_smoothSpeed * delta;
	m_actualY -= (m_actualY - m_targetY) * m_smoothSpeed * delta;
	// and smoothly zoom in
	m_actualScale -= (m_actualScale - m_targetScale) * m_zoomSpeed * delta;
}

// takes x,y as screen coordinates and translates them to world coordinates
void Camera::screenToWorld(float* x, float* y)
{
	float screenWidth = (float)m_game->getWindowWidth();
	float screenHeight = (float)m_game->getWindowHeight();

	float xPercentage = *x / screenWidth;
	float yPercentage = *y / screenHeight;

	float scaledWidth = screenWidth * m_actualScale;
	float scaledHeight = screenHeight * m_actualScale;

	float midX = m_actualX + (screenWidth / 2.0f);
	float midY = m_actualY + (screenHeight / 2.0f);

	float left = midX - (scaledWidth / 2.0f);
	float bottom = midY - (scaledHeight / 2.0f);

	*x = left + (xPercentage * scaledWidth);
	*y = bottom + (yPercentage * scaledHeight);
}

// getters and setters

void Camera::setPosition(float x, float y)
{
	m_targetX = x;
	m_targetY = y;
}

// puts the x and y position into the provided floats
void Camera::getPosition(float* x, float* y)
{
	*x = m_actualX;
	*y = m_actualY;
}

void Camera::setX(float x) { m_targetX = x; }
void Camera::setY(float y) { m_targetY = y; }

float Camera::getX() { return m_actualX; }
float Camera::getY() { return m_actualY; }

void Camera::setScale(float s) { m_targetScale = s; }
float Camera::getScale() { return m_actualScale; }

void	Camera::setSpeed(float spd) { m_smoothSpeed = spd; }
float	Camera::getSpeed() { return m_smoothSpeed; }

void	Camera::setZoomSpeed(float spd) { m_zoomSpeed = spd; }
float	Camera::getZoomSpeed() { return m_zoomSpeed; }
