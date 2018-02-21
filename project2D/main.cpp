#include "game.h"

int main()
{

	// allocation
	auto app = new Game();

	// initialise and loop
	app->run("isotest", 1280, 720, false);

	// deallocation
	delete app;

	return 0;
}