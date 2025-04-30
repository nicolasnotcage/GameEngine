#ifndef PLATFORM_AUDIO_ENGINE_HPP
#define PLATFORM_AUDIO_ENGINE_HPP

#include "fmod/fmod.hpp"
#include "platform/math.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace cge
{

class AudioComponent;

/*
* A Singleton audio engine. Used to manage FMOD
* within the game. 
*/
class AudioEngine
{
public:

    // Singleton getter
    static AudioEngine* get_instance();

    // Initialize and shutdown FMOD
    bool init(int max_channels = 32, bool use_3d = false);
    void shutdown();

    // Load a sound into the engine. Requires a path and an 
    // associated key. Optionally accepts a flag indicating 
    // whether the sound is 3D and whether the sound should 
    // loop. 3D and Looping flags default to false.
    bool load_sound(const std::string &path, 
                    const std::string &key, 
                    bool is_3d = false,
                    bool looping = false);

    // Unload a sound from the engine with a key.
    void unload_sound(const std::string &key);

    // Get a pointer to a sound by key.
    FMOD::Sound* get_sound(const std::string& key);
   
    // Play sounds by key. Accepts optional argument for volume and whether or not 
    // the sound should begin in a paused state.
    int play_sound(const std::string &key, float volume = 1.0f, bool pause = false);

    // Add an echo effect to the specified channel. Accepts optional arguments to
    // configure echo time and feedback amount.
    void add_echo(int channel_id, float delay_ms = 500.0f, float feedback = 50.0f);

    // Set the position of a 3D audio listener by passing in a Vector2. 3D 
    // sounds are played relative to the position of the listener.
    void set_3d_listener_position(const Vector2 &position);

    // Update the audio engine.
    void update();

    // Reserve a channel for a sound using the sound key.
    void reserve_channel_for_sound(const std::string &sound_key, int channel_id);

    // Return the next available channel in the engine. Use this to ensure that 
    // you don't override an existing channel.
    int  get_next_available_channel();

    // Update the statuses of the engine's channels. Handles channel cleanup.
    void update_channel_statuses();

    // Get a pointer to a channel by id.
    FMOD::Channel *get_channel(int channel_id);

    // Get a poiner to the FMOD system.
    FMOD::System  *get_system() { return fmod_system_; }

    // For user configuration. This probably shouldn't be in the engine itself because it's 
    // specific to theme music, but it works fine.
    void toggle_music();

private:
    AudioEngine();
    ~AudioEngine();

    // Delete copy and move constructors
    AudioEngine(const AudioEngine &) = delete;
    AudioEngine &operator=(const AudioEngine &) = delete;
    AudioEngine(AudioEngine &&) = delete;
    AudioEngine &operator=(AudioEngine &&) = delete;

    // FMOD Objects
    FMOD::System *                                  fmod_system_{nullptr};  // FMDO System
    std::unordered_map<std::string, FMOD::Sound *>  sound_map_;             // A map of keys to Sounds
    std::unordered_map<std::string, int>            reserved_channels_;     // A map of keys to channel IDs
    std::vector<FMOD::Channel *>                    channels_;              // A collection of channels
    int                                             num_channels_{32};      // The number of channels managed by the engine
};

} // namespace cge

#endif // PLATFORM_AUDIO_ENGINE_HPP
