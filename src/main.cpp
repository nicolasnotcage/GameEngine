/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/core.hpp"
#include "platform/event.hpp"
#include "system/file_locator.hpp"
#include "system/preprocessor.h"

#include "examples/dynamic_scene.hpp"
#include "examples/hybrid_scene.hpp"
#include "examples/static_scene.hpp"

#include <chrono>
#include <iostream>
#include <thread>

void sleep(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int main(int argc, char *argv[])
{
    // Configure system paths and initialize SDL
    auto source_path = STD_STRING(SRC_DIR);
    auto resource_path = STD_STRING(RESOURCE_DIR);
    cge::set_system_paths(argv[0], source_path, resource_path);
    cge::init_sdl();

    // Generate default SDLInfo struct (contains pointers to an SDL renderer and SDL window) 
    cge::SDLInfo sdl_info;

    constexpr int SCREEN_WIDTH = 800;
    constexpr int SCREEN_HEIGHT = 600;

    // Create and configure components of SDL instance
    cge::create_sdl_components(sdl_info, SCREEN_WIDTH, SCREEN_HEIGHT, "Class 605.688");

    // Initialize the active scene
    cge::StaticScene scene;

    // Initialize scene with reference to SDLInfo struct
    scene.init(&sdl_info);

    bool run_game = true;
    while(run_game)
    {
        auto curr_events = cge::get_current_events();
        for(uint8_t i = 0; i < curr_events.num_events; ++i)
        {
            switch(curr_events.events[i])
            {
                case cge::EventType::QUIT: // 'esc' or close window
                    run_game = false;
                    break;
                default: break;
            }
        }

        scene.render();
        sleep(16);
    }

    scene.destroy();
    cge::destroy_sdl_components(sdl_info);
    return 0;
}
