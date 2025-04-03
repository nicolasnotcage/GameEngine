/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "system/config_manager.hpp"
#include "system/file_locator.hpp"
#include "system/string_utils.hpp"
#include <iostream>
#include <fstream>

namespace cge
{
ConfigManager& ConfigManager::get_instance()
{
    static ConfigManager instance;
    return instance;
}

// TODO: Add a destroy() method to handle this behavior
ConfigManager::~ConfigManager()
{
    // Save config when destroyed
    if (is_loaded_)
    {
        save();
    }
}

bool ConfigManager::init(const std::string& config_filepath)
{
    // Check if file exists
    auto file_info = locate_path_for_filename(config_filepath);

    if (file_info.found) 
    {
        // Use file path if found
        config_filepath_ = file_info.path;
    }
    else
    {
        // Use the resources directory for the config file
        // TODO: Write to a more appropriate directory
        config_filepath_ = "../resources/" + config_filepath;
    }

    // Check if file exists
    std::ifstream check_file(config_filepath_);
    bool file_exists = check_file.good();
    check_file.close();

    if (!file_exists)
    {
        // Create default config if file doesn't exist
        return create_default_config();
    }

    // File exists, try to load it
    return load();
}

bool ConfigManager::save()
{
    if (!serializer_.open(config_filepath_, true))
    {
        std::cerr << "Failed to open config file for writing: " << config_filepath_ << std::endl;
        return false;
    }

    // Write all config values
    serializer_.write("screen_width", screen_width_);
    serializer_.write("screen_height", screen_height_);
    serializer_.write("music_enabled", music_enabled_);
    serializer_.write("test_int", test_int_);
    serializer_.write("test_float", test_float_);
    serializer_.write("test_bool", test_bool_);
    serializer_.write("test string", test_string_);

    bool result = serializer_.save();
    serializer_.close();

    return result;
}

bool ConfigManager::load()
{
    if (!serializer_.open(config_filepath_, false))
    {
        std::cerr << "Failed to open config file for reading: " << config_filepath_ << std::endl;
        return false;
    }

    // Read all config values, keep default if not found
    serializer_.read("screen_width", screen_width_);
    serializer_.read("screen_height", screen_height_);
    serializer_.read("music_enabled", music_enabled_);
    serializer_.read("test_int", test_int_);
    serializer_.read("test_float", test_float_);
    serializer_.read("test_bool", test_bool_);
    serializer_.read("test_string", test_string_);

    serializer_.close();
    is_loaded_ = true;

    return true;
}

bool ConfigManager::create_default_config()
{
    // Reset to default values
    screen_width_ = 800;
    screen_height_ = 600;
    music_enabled_ = true;
    test_int_ = 0;
    test_float_ = 0.0;
    test_bool_ = true;
    test_string_ = "test";

    // Save the defaults
    bool result = save();
    is_loaded_ = result;

    return result;
}
    
int ConfigManager::get_screen_width() const
{
    return screen_width_;
}
    
void ConfigManager::set_screen_width(int width)
{
    screen_width_ = width;
}
    
int ConfigManager::get_screen_height() const
{
    return screen_height_;
}
    
void ConfigManager::set_screen_height(int height)
{
    screen_height_ = height;
}

bool ConfigManager::get_music_enabled() const
{
    return music_enabled_;
}

void ConfigManager::set_music_enabled(bool enabled)
{
    music_enabled_ = enabled;
}

void ConfigManager::increment_test_values()
{
    test_int_ += 1;
    test_float_ += 0.1;
    test_bool_ = !test_bool_;
    
    if (test_bool_) test_string_ = cge::utility::to_lower(test_string_);
    else test_string_ = cge::utility::to_upper(test_string_);
}

} // namespace cge
