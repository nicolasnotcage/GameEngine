#include "io_handler.hpp"

#include "event.hpp"

namespace cge
{

// Update stored events
void IoHandler::update() { curr_events_ = cge::get_current_events(); }   

// Check if a quit was requested
bool IoHandler::quit_requested() const
{
    for(uint8_t i = 0; i < curr_events_.num_events; ++i)
    {
        if(curr_events_.events[i] == cge::EventType::QUIT) return true;
    }
    return false;
}

} // namespace cge

