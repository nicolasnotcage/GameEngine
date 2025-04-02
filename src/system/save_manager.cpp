/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "system/save_manager.hpp"
#include "platform/scene.hpp"
#include "graph/transform_node.hpp"
#include "system/file_locator.hpp"
#include <iostream>
#include <fstream>

namespace cge
{

SaveManager* SaveManager::instance_ = nullptr;

SaveManager* SaveManager::get_instance()
{
    if (instance_ == nullptr)
    {
        instance_ = new SaveManager();
    }
    return instance_;
}

SaveManager::~SaveManager()
{
    // Ensure any pending saves are completed
    if (is_loaded_)
    {
        serializer_.close();
    }
}

bool SaveManager::init(const std::string& save_filepath)
{
    // First check if the file exists using file_locator
    auto file_info = locate_path_for_filename(save_filepath);
    if (file_info.found)
    {
        // Use the full path from file_locator
        save_filepath_ = file_info.path;
        std::cout << "Found existing save file at: " << save_filepath_ << std::endl;
    }
    else
    {
        // Use the resources directory for the save file
        save_filepath_ = "../resources/" + save_filepath;
        std::cout << "Will create new save file at: " << save_filepath_ << std::endl;
    }
    
    is_loaded_ = true;
    return true;
}

bool SaveManager::save_game(const Scene* scene)
{
    if (!scene || !is_loaded_)
    {
        return false;
    }

    if (!serializer_.open(save_filepath_, true))
    {
        std::cerr << "Failed to open save file for writing: " << save_filepath_ << std::endl;
        return false;
    }

    // Save game state by delegating to the Scene
    scene->serialize(serializer_);
    
    // Add a timestamp to the save file
    serializer_.write("save_timestamp", static_cast<int>(time(nullptr)));

    bool result = serializer_.save();
    serializer_.close();
    
    std::cout << "Game saved successfully to: " << save_filepath_ << std::endl;
    return result;
}

bool SaveManager::load_game(const Scene* scene)
{
    if (!scene || !is_loaded_)
    {
        return false;
    }

    if (!save_exists())
    {
        std::cerr << "No save file found at: " << save_filepath_ << std::endl;
        return false;
    }

    if (!serializer_.open(save_filepath_, false))
    {
        std::cerr << "Failed to open save file for reading: " << save_filepath_ << std::endl;
        return false;
    }

    // Load game state by delegating to the Scene
    // We need to const_cast because deserialize is non-const
    const_cast<Scene*>(scene)->deserialize(serializer_);
    
    // Read the timestamp for logging
    int timestamp;
    if (serializer_.read("save_timestamp", timestamp))
    {
        std::cout << "Loaded game state from: " << timestamp << std::endl;
    }

    serializer_.close();
    
    std::cout << "Game loaded successfully from: " << save_filepath_ << std::endl;
    return true;
}

bool SaveManager::save_exists() const
{
    std::ifstream check_file(save_filepath_);
    bool file_exists = check_file.good();
    check_file.close();

    return file_exists;
}

} // namespace cge
