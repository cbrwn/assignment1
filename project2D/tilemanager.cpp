#include <Input.h>

#include "game.h"
#include "tile.h"
#include "tilemanager.h"

TileManager::TileManager(Game* game, Tile**** tiles)
	: m_game(game), m_tiles(tiles)
{
	m_selectedType = ZONETYPE_NONE;
	m_dragging = false;
}

void TileManager::updateZoneEditing()
{
	aie::Input* input = aie::Input::getInstance();
	// Z to toggle mode
	if (input->wasKeyPressed(aie::INPUT_KEY_Z))
	{
		m_game->setPlaceMode(PlaceMode::PLACEMODE_NONE);
		return;
	}

	// zone creation
	if (!m_dragging 
		&& input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT)
		&& m_game->isMouseInGame())
	{
		getTileAtMousePosition(&m_dragStartX, &m_dragStartY);
		// only start dragging if we're within the bounds of the world
		if(m_dragStartX >= 0 && m_dragStartY >= 0)
			m_dragging = true;
	}

	if (m_dragging)
	{
		if (!input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_LEFT))
		{
			// stop dragging and zone the selected tiles
			m_dragging = false;

			// this is gross, but making a function for it is also gross so idk
			// swap the start/end if the rectangle is backwards
			int dragMinX = m_dragStartX; int dragMinY = m_dragStartY;
			int dragMaxX = m_dragEndX; int dragMaxY = m_dragEndY;
			if (dragMinX > dragMaxX)
			{
				dragMinX = m_dragEndX;
				dragMaxX = m_dragStartX;
			}
			if (dragMinY > dragMaxY)
			{
				dragMinY = m_dragEndY;
				dragMaxY = m_dragStartY;
			}

			for (int y = dragMinY; y <= dragMaxY; y++)
			{
				for (int x = dragMinX; x <= dragMaxX; x++)
				{
					Tile* t = (*m_tiles)[y][x];
					if (!t)
						continue;
					t->setZoneType(m_selectedType);
				}
			}
		}
		else
		{
			// update the dragging rectangle
			getTileAtMousePosition(&m_dragEndX, &m_dragEndY);
			// make sure we're not selecting outside the bounds of the world
			if (m_dragEndX < 0)
				m_dragEndX = 0;
			if (m_dragEndY < 0)
				m_dragEndY = 0;
		}
	}
}

void TileManager::drawZoneSelection(aie::Renderer2D* renderer)
{
	// draw current selection
	if (m_dragging)
	{
		renderer->setRenderColour(1, 1, 1);
		m_game->drawTileRect(m_dragStartX, m_dragStartY, m_dragEndX, 
			m_dragEndY);
	}
}

void TileManager::getTileAtMousePosition(int* ix, int* iy)
{
	// first get the mouse's world position
	Vector2 mousePos = m_game->getMouseWorldPosition();

	// then grab the tile's indices
	getTileAtPosition(mousePos, ix, iy);
}

Tile* TileManager::getTileAtPosition(Vector2& pos)
{
	int ix, iy;
	getTileAtPosition(pos, &ix, &iy);

	if (ix < 0 || iy < 0)
		return nullptr;
	return (*m_tiles)[iy][ix];
}

void TileManager::getTileAtPosition(Vector2& pos, int* ix, int* iy)
{
	// get shorter names for the measurements we use to offset the tiles
	const float tw = TILE_WIDTH / 2.0f;
	const float th = TILE_HEIGHT / 3.0f;

	// grab local versions of the vector position
	float px = pos.getX();
	float py = pos.getY();

	// invert y since y=0 is at the bottom
	py *= -1;

	// offset to the start of the map
	Vector2 mapStart = m_game->getMapStart();
	px -= mapStart.getX();
	py += mapStart.getY();

	// account for the origin of the tile drawing (see tile.cpp)
	px -= tw;
	py += th;

	// the inverse of the calculations in getTileWorldPosition
	int resultX = (int)((px / tw + py / th) / 2);
	int resultY = (int)((py / th - px / tw) / 2);

	if (resultX < 0 || resultX >= WORLD_WIDTH)
		resultX = -1;
	if (resultY < 0 || resultY >= WORLD_HEIGHT)
		resultY = -1;

	*ix = resultX;
	*iy = resultY;
}

Vector2 TileManager::getTileWorldPosition(int ix, int iy)
{
	Vector2 mapStart = m_game->getMapStart();
	float xpos = (ix - iy) * TILE_WIDTH / 2.0f + mapStart.getX();
	float ypos = (ix + iy) * TILE_HEIGHT / 3.0f - mapStart.getY();
	return Vector2(xpos, -ypos);
}

void TileManager::clearTilePower()
{
	for (int y = 0; y < WORLD_HEIGHT; ++y)
	{
		for (int x = 0; x < WORLD_WIDTH; ++x)
		{
			Tile* t = getTile(x, y);
			if (!t || !t->hasPower())
				continue;
			t->takePower();
		}
	}
}
