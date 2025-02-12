#include "game_manager.hpp"

#include <chrono>
#include <iostream>
#include <thread> 

namespace cge
{
// Define game manager
GameManager *GameManager::game_manager_ = nullptr;

// Get instance of class; if not initialized, create new.
GameManager* GameManager::get_instance() 
{ 
	if(game_manager_ == nullptr) game_manager_ = new GameManager();
	return game_manager_;
}

void GameManager::sleep(int milliseconds) { std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds)); }

} // namespace cge

