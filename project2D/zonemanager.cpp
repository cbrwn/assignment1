#include <Input.h>

#include "game.h"
#include "tile.h"
#include "zonemanager.h"

ZoneManager::ZoneManager(Game* game, Tile**** tiles)
	: m_game(game), m_tiles(tiles)
{
	m_selectedType = ZONETYPE_NONE;
	m_dragging = false;
}

void ZoneManager::zoneMode()
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
		m_game->getTileAtMousePosition(&m_dragStartX, &m_dragStartY);
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
			m_game->getTileAtMousePosition(&m_dragEndX, &m_dragEndY);
			// make sure we're not selecting outside the bounds of the world
			if (m_dragEndX < 0)
				m_dragEndX = 0;
			if (m_dragEndY < 0)
				m_dragEndY = 0;
		}
	}
}

void ZoneManager::drawZones(aie::Renderer2D* renderer)
{
	// draw current selection
	if (m_dragging)
	{
		renderer->setRenderColour(1, 1, 1);
		m_game->drawTileRect(m_dragStartX, m_dragStartY, m_dragEndX, 
			m_dragEndY);
	}
}
