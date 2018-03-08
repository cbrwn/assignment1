#pragma once

class Game;

#define SAVEFILE_NAME "city.wld"

class SaveManager
{
public:
	SaveManager(Game* game);

	bool loadData();
	bool saveData();
private:
	Game* m_game;
};