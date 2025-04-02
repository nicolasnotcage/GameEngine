#ifndef SERIALIZATION_SAVE_MANAGER_HPP
#define SERIALIZATION_SAVE_MANAGER_HPP

#include "system/serializer.hpp"

#include <string>

namespace cge
{

class Scene;

class SaveManager
{
public:
    static SaveManager* get_instance();

    // Initialize with path to save file
    bool init(const std::string& save_filepath);

    // Save game state
    bool save_game(const Scene* scene);

    // Load game state into a scene
    bool load_game(const Scene *scene);

    // Check if save file exists
    bool save_exists() const;

private:
    SaveManager() = default;
    ~SaveManager();

    static SaveManager* instance_;
    BinarySerializer serializer_;
    std::string save_filepath_;
    bool is_loaded_ = false;
};
} // namespace cge

#endif // SERIALIZATION_SAVE_MANAGER_HPP
