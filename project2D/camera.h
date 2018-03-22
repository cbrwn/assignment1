#pragma once

class Game;

class Camera
{
public:
	Camera(Game* game);

	void update(float delta);

	void screenToWorld(float* x, float* y);

	// these set the target positions - the positions the camera will smoothly 
	// move to
	void setPosition(float x, float y);
	void setX(float x);
	void setY(float y);
	void setScale(float s);
	// sets the CURRENT scale, not the target scale
	void setCurrentScale(float s);

	// these will get the actual values, not the destinations which are set 
	// above
	float getX();
	float getY();
	float getScale();
	float getTargetScale();
	void  getPosition(float* x, float* y);

	void  setSpeed(float spd);
	float getSpeed();

	void  setZoomSpeed(float spd);
	float getZoomSpeed();

	inline void setShakeAmount(float amt) { m_shakeAmount = amt; }
	inline float getShakeAmount() { return m_shakeAmount; }
private:
	Game* m_game;

	// destination positions
	float m_targetX, m_targetY;
	float m_targetScale;
	// actual positions
	float m_actualX, m_actualY;
	float m_actualScale;
	float m_smoothSpeed, m_zoomSpeed;
	float m_arrowSpeed; // speed to move the camera with arrow keys

	float m_shakeAmount;

	// dragging control
	bool  m_dragging;
	float m_dragStartMX, m_dragStartMY; // starting mouse position
	float m_dragStartCX, m_dragStartCY; // starting camera pos

	// zoom control
	double m_lastScroll;
};