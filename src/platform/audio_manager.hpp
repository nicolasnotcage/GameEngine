/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

/*
This header defines an AudioManager class that abstracts audio handling.
It manages sound loading, 3D audio positioning, and cross-entity coordination.
*/

#ifndef PLATFORM_AUDIO_MANAGER_HPP
#define PLATFORM_AUDIO_MANAGER_HPP

#include "platform/audio_engine.hpp"
#include "graph/scene_state.hpp"
#include "system/file_locator.hpp"

#include <string>
#include <memory>
#include <vector>

namespace cge
{

class Player;
class NPC;

class AudioManager
{
public:
    AudioManager();
    ~AudioManager() = default;
    
    void init(SceneState& scene_state);
    
    // Load and configure sounds
    void load_sounds();
    
    // Update 3D audio positioning
    void update_audio_positions(std::shared_ptr<Player> player, std::shared_ptr<NPC> npc);
    
    // Play specific sounds
    void play_sound(const std::string& sound_name, float volume = 1.0f);
    
    // Toggle music
    void toggle_music();
    
private:
    // Helper method to locate audio files
    std::string locate_audio_file(const std::string& filename);
    
    // Scene state reference
    SceneState* scene_state_{nullptr};
};

} // namespace cge

#endif // PLATFORM_AUDIO_MANAGER_HPP
