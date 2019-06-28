// main.cpp

#include "game_manager.hpp"
#include "gridspace.hpp"
#include "minkowski.hpp"
#include "agent.hpp"
#include "craft.hpp"
#include "wrap.hpp"

int main(int argc, char* argv[])
{
	// Order of world registration of game objects, is their order in the savefile.
	isk::Wrap<iskandria::grid::cartesian_2d>::world_setup();
	iskandria::minkowski::world_setup();

	iskandria::agent::world_setup();
	iskandria::craft::world_setup();

	// start the game engine
	isk::GameManager::get().run();
	return 0;
}

