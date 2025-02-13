#include "time_manager.hpp"

#include <chrono>

namespace cge
{

TimeManager::TimeManager() 
{ 
	last_time_ = get_current_time();
	delta_time_ = 0.0;
}

TimeManager::~TimeManager() = default;

// Update last delta and last time
void TimeManager::update()
{ 
	auto current_time = get_current_time();
    delta_time_ = current_time - last_time_;
    last_time_ = current_time;
}

// Return delta time
double TimeManager::get_delta_time() const { return delta_time_; }

// In time_manager.cpp
double TimeManager::get_current_time() const
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(my_clock.now().time_since_epoch()).count();
}

} // namespace cge

