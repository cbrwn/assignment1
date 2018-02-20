#pragma once

/*
	Camera
	This class controls everything to do with the camera
		including movement and player controls
*/

class Camera
{
public:
	Camera();

	void update(float delta);

	void screenToWorld(float* x, float* y);

	// these set the target positions - the positions the camera will smoothly move to
	void setPosition(float x, float y);
	void setX(float x);
	void setY(float y);
	void setScale(float s);

	// these will get the actual values, not the destinations which are set above
	float getX();
	float getY();
	float getScale();
	void getPosition(float* x, float* y);

	void setSpeed(float spd);
	float getSpeed();

	void setZoomSpeed(float spd);
	float getZoomSpeed();
private:
	// destination positions
	float m_targetX, m_targetY;
	float m_targetScale;
	// actual positions
	float m_actualX, m_actualY;
	float m_actualScale;
	float m_smoothSpeed, m_zoomSpeed;
	float m_arrowSpeed; // speed to move the camera with arrow keys

	// dragging control
	bool m_dragging;
	float m_dragStartX, m_dragStartY;
};