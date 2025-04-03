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

SaveManager& SaveManager::get_instance()
{
    static SaveManager instance;
    return instance;
}

// TODO: Add a destroy() method to handle this behavior.
SaveManager::~SaveManager()
{
    if (is_loaded_)
    {
        serializer_.close();
    }
}

bool SaveManager::init(const std::string& save_filepath)
{
    // Check if the file exists
    auto file_info = locate_path_for_filename(save_filepath);
    if (file_info.found)
    {
        // Use the full path
        save_filepath_ = file_info.path;
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
    if (!scene || !is_loaded_) return false;

    if (!serializer_.open(save_filepath_, true))
    {
        std::cerr << "Failed to open save file for writing: " << save_filepath_ << std::endl;
        return false;
    }

    // Save game state by delegating to the Scene
    scene->serialize(serializer_);

    // Save and close serializer
    bool result = serializer_.save();
    serializer_.close();
    
    return result;
}

bool SaveManager::load_game(const Scene* scene)
{
    if (!scene || !is_loaded_) return false;

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
    const_cast<Scene*>(scene)->deserialize(serializer_);    
    serializer_.close();
    
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
