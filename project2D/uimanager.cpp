#include <Input.h>
#include <Font.h>

#include "game.h"
#include "uimanager.h"

#include "tile.h"

UiManager::UiManager(Game* game) : m_game(game), m_panelColour(0x444444ff)
{
	m_panelHeight = 100.0f;

	m_tileImages = new aie::Texture*[4];
	m_tileImages[GRASS] = game->getImageManager()->getTexture("tiles/grass_flat");
	m_tileImages[HOUSE] = game->getImageManager()->getTexture("tiles/house_left");
	m_tileImages[MILITARY] = game->getImageManager()->getTexture("tiles/military_left");
	m_tileImages[SHOP] = game->getImageManager()->getTexture("tiles/shop_left");

	m_boxPositionY = m_panelHeight / 2.0f;
	m_boxWidth = 84.0f;
	m_mouseOverIndex = -1;
}

UiManager::~UiManager()
{
	delete[] m_tileImages;
}

void UiManager::update(float delta)
{
	// update the positions of the selection boxes
	const float selectionBoxStartX = 48.0f;
	for (int i = 0; i < UI_SELECTION_BOX_COUNT; i++)
		m_boxPositions[i] = selectionBoxStartX + (m_boxWidth * i);

	aie::Input* input = aie::Input::getInstance();
	int mx, my;
	input->getMouseXY(&mx, &my);
	m_mouseOverIndex = -1;
	if (isMouseOverUi())
	{
		// panel was clicked - check if a box was clicked
		for (int i = 0; i < UI_SELECTION_BOX_COUNT; i++)
		{
			if (isInsideBox(mx, my, i))
			{
				m_mouseOverIndex = i;
				if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT))
					// this box was clicked!
					m_game->setPlaceMode(i);
				break;
			}
		}
	}
}

void UiManager::draw(aie::Renderer2D* renderer)
{
	// base panel box
	renderer->setRenderColour(m_panelColour);
	renderer->drawBox(m_game->getWindowWidth() / 2.0f, m_panelHeight / 2.0f,
		(float)m_game->getWindowWidth(), m_panelHeight);

	// box behind panel title
	renderer->drawBox(32, m_panelHeight, 64, 24);
	// panel title
	renderer->setRenderColour(1, 1, 1);
	renderer->drawText(m_game->m_uiFontLarge, "Build", 8, m_panelHeight - 8);

	aie::Input* input = aie::Input::getInstance();
	int mx, my;
	input->getMouseXY(&mx, &my);

	const float selectionImageScale = 0.5f;
	for (int i = 0; i < UI_SELECTION_BOX_COUNT; i++)
	{
		float imagePosX = m_boxPositions[i];
		float imagePosY = m_boxPositionY;

		// grab the image and scale it down to fit
		aie::Texture* thisImage = m_tileImages[i];
		float imageWidth = thisImage->getWidth() * selectionImageScale;
		float imageHeight = thisImage->getHeight() * selectionImageScale;

		// show rectangle to indicate if this tile is selected
		if (m_game->getPlaceMode() == i)
		{
			renderer->setRenderColour(0x666666ff);
			renderer->drawBox(imagePosX, imagePosY, imageWidth*1.2f, imageHeight*1.2f);
			// reset our colour back to white to draw the image properly
			renderer->setRenderColour(1, 1, 1);
		}

		// show name of tile if mouse is over
		if (m_mouseOverIndex == i)
		{
			char* name = m_game->getTileName((TileType)i);
			float nameWidth = m_game->m_uiFont->getStringWidth(name);
			renderer->drawText(m_game->m_uiFont, name, imagePosX - nameWidth/2.0f, 
				m_panelHeight - 16);
		}

		renderer->drawSprite(thisImage, imagePosX, imagePosY, imageWidth, imageHeight);
	}
}

bool UiManager::isMouseOverUi()
{
	int mx, my;
	aie::Input::getInstance()->getMouseXY(&mx, &my);
	return my < m_panelHeight;
}

bool UiManager::isInsideBox(int x, int y, int boxIndex)
{
	float boxPosX = m_boxPositions[boxIndex] - m_boxWidth / 2.0f;
	float boxPosY = m_boxPositionY;
	bool isInside = (x > boxPosX - m_boxWidth && x < boxPosX + m_boxWidth &&
		y > boxPosY - m_boxWidth && y < boxPosY + m_boxWidth);
	return isInside;
}
