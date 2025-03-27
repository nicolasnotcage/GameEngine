/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/core.hpp"
#include "platform/event.hpp"
#include "system/file_locator.hpp"
#include "system/preprocessor.h"

#include "examples/main_scene.hpp"

#include "platform/game_manager.hpp"
#include "platform/io_handler.hpp"
#include "platform/time_manager.hpp"
#include "platform/config.hpp"
#include "platform/scene_manager.hpp"

#include "fmod/fmod.hpp"

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

    // Create and configure components of SDL instance
    cge::create_sdl_components(sdl_info, cge::SCREEN_WIDTH, cge::SCREEN_HEIGHT, "Class 605.688");

    // Create io and time handler instances
    cge::IoHandler    io_handler = cge::IoHandler();
    cge::TimeManager *time_manager = cge::TimeManager::get_instance();

    // Initialize the scene manager
    cge::SceneManager *scene_manager = cge::SceneManager::get_instance();
    scene_manager->init(&sdl_info, &io_handler);

    // Create and push the main scene
    cge::MainScene *main_scene = new cge::MainScene();
    scene_manager->push_scene(main_scene);

    // Get instance of game manager class
    auto game_manager = cge::GameManager::get_instance();

    // Main game loop
    bool run_game = true;
    while (run_game)
    {
        // Run game loop with scene manager
        game_manager->run_game_loop(*scene_manager, io_handler);

        // See if a quit was requested
        if(io_handler.quit_requested()) run_game = false;
    }
    
    // Cleanup after game loop
    scene_manager->clear_all_scenes();
    delete main_scene;
    cge::destroy_sdl_components(sdl_info);
    return 0;
}
