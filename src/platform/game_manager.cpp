#include "game_manager.hpp"
#include "time_manager.hpp"

#include <chrono>
#include <iostream>
#include <thread> 

namespace cge
{
// Initialize game manager
GameManager *GameManager::game_manager_ = nullptr;

// Initialize static variables
uint64_t GameManager::last_update_time_ = 0;
uint64_t GameManager::last_draw_time_ = 0;

GameManager::GameManager()
{	
    time_manager_ = new TimeManager();
}

GameManager::~GameManager()
{
    delete time_manager_;
}

// Get instance of class; if not initialized, create new.
GameManager* GameManager::get_instance() 
{ 
	if(game_manager_ == nullptr) game_manager_ = new GameManager();
	return game_manager_;
}

void GameManager::sleep(int milliseconds) { std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds)); }

} // namespace cge

