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

#include "system/config_manager.hpp"
#include "system/save_manager.hpp"

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

    // Initialize config manager first to get screen size and other settings
    cge::ConfigManager* config_manager = cge::ConfigManager::get_instance();
    if (config_manager->init("config.txt")) 
    {
        std::cout << "Config loaded successfully" << std::endl;
        std::cout << "Target frame rate: " << config_manager->get_target_frame_rate() << std::endl;
        std::cout << "Screen size: " << config_manager->get_screen_width() << "x" 
                  << config_manager->get_screen_height() << std::endl;
    }

    // Generate default SDLInfo struct (contains pointers to an SDL renderer and SDL window) 
    cge::SDLInfo sdl_info;

    // Create and configure components of SDL instance using config values
    cge::create_sdl_components(sdl_info, 
                              config_manager->get_screen_width(), 
                              config_manager->get_screen_height(), 
                              "Class 605.688");

    // Create io and time handler instances
    cge::IoHandler    io_handler = cge::IoHandler();
    cge::TimeManager *time_manager = cge::TimeManager::get_instance();

    // Initialize the scene manager
    cge::SceneManager *scene_manager = cge::SceneManager::get_instance();
    scene_manager->init(&sdl_info, &io_handler);

    // Initialize save manager
    cge::SaveManager* save_manager = cge::SaveManager::get_instance();
    if (save_manager->init("save.dat")) 
    {
        std::cout << "Save manager initialized" << std::endl;
    }

    // Create the main scene
    cge::MainScene *main_scene = new cge::MainScene();
    
    // Load any saved data before initializing the scene
    if (save_manager->save_exists())
    {
        std::cout << "Loading saved game data..." << std::endl;
        save_manager->load_game(main_scene);
        
        // Access the config manager to display loaded values
        std::cout << "Loaded config values:\n";
        std::cout << "  Target frame rate: " << config_manager->get_target_frame_rate() << "\n";
    }
    else
    {
        std::cout << "No save file found. Starting with default values.\n";
    }
    
    // Now push the scene to the manager
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

    // Save game state after loop ends
    save_manager->save_game(main_scene);
    
    // Cleanup after game loop
    scene_manager->clear_all_scenes();
    delete main_scene;
    cge::destroy_sdl_components(sdl_info);
    return 0;
}
