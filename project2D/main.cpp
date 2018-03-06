// stuff to detect memory leaks
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

#include "game.h"

int main()
{
	// allocation
	auto app = new Game();

	// initialise and loop
	app->run("isotest", 854, 640, false);

	// deallocation
	delete app;

	// log any detected leaks
	_CrtDumpMemoryLeaks();
	return 0;
}