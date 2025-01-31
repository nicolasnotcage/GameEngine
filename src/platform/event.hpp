/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef PLATFORM_INPUT_HPP
#define PLATFORM_INPUT_HPP

#include <cstddef>
#include <cstdint>

namespace cge
{

enum class EventType
{
    PLAY_SOUND,
    TOGGLE_MUSIC,
    QUIT
};

struct SDLEventInfo
{
    static constexpr size_t MAX_EVENTS = 10;
    uint8_t                 num_events;
    EventType               events[MAX_EVENTS];
};

SDLEventInfo get_current_events();

} // namespace cge

#endif // PLATFORM_INPUT_HPP
