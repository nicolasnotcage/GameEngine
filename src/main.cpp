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


#include "platform/game_manager.hpp"
#include "platform/io_handler.hpp"
#include "platform/time_manager.hpp"

#include <chrono>
#include <iostream>
#include <thread>

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
    scene.init(&sdl_info);

    // Get instance of game manager class and run the game loop
    auto game_manager = cge::GameManager::get_instance();

    // Create io and time handler instances
    cge::IoHandler io_handler = cge::IoHandler();
    cge::TimeManager *time_manager = cge::TimeManager::get_instance();

    // Measure start time
    double start_time = time_manager->get_current_time();
    std::cout << "Time at loop start: " << start_time << std::endl;

    // Main game loop
    bool run_game = true;
    while (run_game)
    {
        // Update IO events and see if a quit was requested
        io_handler.update();
        if(io_handler.quit_requested()) run_game = false;

        // Run game loop with scene
        game_manager->run_game_loop<cge::StaticScene>(scene);
    }

    // Calculate end time and duration
    double end_time = time_manager->get_current_time();
    std::cout << "Time at loop end: " << end_time << std::endl;
    std::cout << "Total elapsed time: " << end_time - start_time << "s..." << std::endl;
    
    // Cleanup after game loop
    scene.destroy();
    cge::destroy_sdl_components(sdl_info);
    return 0;
}
