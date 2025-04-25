/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/player.hpp"
#include "examples/npc.hpp"
#include "platform/audio_manager.hpp"
#include "platform/audio_component.hpp"
#include "system/file_locator.hpp"
#include "system/config_manager.hpp"

namespace cge
{

AudioManager::AudioManager()
{
}

void AudioManager::init(SceneState& scene_state)
{
    scene_state_ = &scene_state;
    
    // Load sounds
    load_sounds();
}

void AudioManager::load_sounds()
{
    AudioEngine* audio_engine = AudioEngine::get_instance();
    
    // Locate files
    auto player_file_info = locate_audio_file("whistle.wav");
    auto npc_file_info = locate_audio_file("npc_clap.wav");
    auto collision_sound_info = locate_audio_file("success.wav");
    auto theme_sound_info = locate_audio_file("theme_music.mp3");

    // Load sounds - note that npc_clap is a 3D sound
    audio_engine->load_sound(player_file_info, "whistle", false, false);
    audio_engine->load_sound(npc_file_info, "npc_clap", true, false); // Set as 3D sound
    audio_engine->load_sound(collision_sound_info, "success", false, false);
    audio_engine->load_sound(theme_sound_info, "theme_music", false, true);

    // Reserve channels for each sound
    audio_engine->reserve_channel_for_sound("success", 0);
    audio_engine->reserve_channel_for_sound("npc_clap", 1);
    audio_engine->reserve_channel_for_sound("whistle", 2);
    audio_engine->reserve_channel_for_sound("theme_music", 3);
    
    // Begin theme music at low volume
    audio_engine->play_sound("theme_music", 0.2f);

    // Mute theme music if configuration set
    bool music_enabled = ConfigManager::get_instance().get_music_enabled();
    if (!music_enabled) audio_engine->get_channel(3)->setMute(true);
}

void AudioManager::update_audio_positions(std::shared_ptr<Player> player, std::shared_ptr<NPC> npc)
{
    if (!player || !npc) return;

    // Update listener position (player position) first
    Vector2 player_position(player->get_position_x(), player->get_position_y());
    AudioEngine::get_instance()->set_3d_listener_position(player_position);

    // Update NPC's audio component position for 3D audio
    if (auto* npc_audio = npc->get_audio_component())
    {
        npc_audio->update_position();
    }
}

void AudioManager::play_sound(const std::string& sound_name, float volume)
{
    AudioEngine::get_instance()->play_sound(sound_name, volume);
}

void AudioManager::toggle_music()
{
    AudioEngine::get_instance()->toggle_music();
}

std::string AudioManager::locate_audio_file(const std::string& filename)
{
    auto file_info = locate_path_for_filename("audio/" + filename);
    return file_info.path;
}

} // namespace cge
