#include "game.h"

int main()
{

	// allocation
	auto app = new Game();

	// initialise and loop
	app->run("isotest", 1920, 1280, false);

	// deallocation
	delete app;

	return 0;
}