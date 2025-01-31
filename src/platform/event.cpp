/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/event.hpp"
#include "platform/sdl.h"

namespace cge
{

SDLEventInfo get_current_events()
{
    SDLEventInfo event_info;
    uint8_t      num_events = 0;

    SDL_Event e;
    while(SDL_PollEvent(&e) && num_events < SDLEventInfo::MAX_EVENTS)
    {
        switch(e.type)
        {
            case SDL_EVENT_MOUSE_BUTTON_DOWN: break;
            case SDL_EVENT_QUIT: event_info.events[num_events++] = EventType::QUIT; break;
            case SDL_EVENT_KEY_DOWN:
                switch(e.key.key)
                {
                    case SDLK_ESCAPE: event_info.events[num_events++] = EventType::QUIT; break;
                    case SDLK_SPACE: event_info.events[num_events++] = EventType::PLAY_SOUND; break;
                    case SDLK_M: event_info.events[num_events++] = EventType::TOGGLE_MUSIC; break;
                    default: break;
                }
                break;
            default: break;
        }
    }

    event_info.num_events = num_events;

    return event_info;
}

} // namespace cge
