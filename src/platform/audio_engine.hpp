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

// A Singleton audio engine to manage 
// an FMOD system. 
class AudioEngine
{
public:

    // Singleton getter
    static AudioEngine* get_instance();

    // Initialize and shutdown FMOD
    bool init(int max_channels = 32, bool use_3d = false);
    void shutdown();

    // Sound resource management
    bool load_sound(const std::string &path, 
                    const std::string &key, 
                    bool is_3d = false,
                    bool looping = false);

    void unload_sound(const std::string &key);
    FMOD::Sound* get_sound(const std::string& key);
   
    // Play sounds
    int play_sound(const std::string &key, float volume = 1.0f, bool pause = false);

    // DSP
    void add_echo(int channel_id, float delay_ms = 500.0f, float feedback = 50.0f);

    // 3D audio listener (attached to player)
    void set_3d_listener_position(const Vector2 &position);

    // System management
    void update();
    void reserve_channel_for_sound(const std::string &sound_key, int channel_id);
    int  get_next_available_channel();
    void update_channel_statuses();
    FMOD::Channel *get_channel(int channel_id);
    FMOD::System  *get_system() { return fmod_system_; }

private:
    AudioEngine();
    ~AudioEngine();

    // Delete copy and move constructors
    AudioEngine(const AudioEngine &) = delete;
    AudioEngine &operator=(const AudioEngine &) = delete;
    AudioEngine(AudioEngine &&) = delete;
    AudioEngine &operator=(AudioEngine &&) = delete;

    // FMOD Objects
    FMOD::System *                                  fmod_system_{nullptr};
    std::unordered_map<std::string, FMOD::Sound *>  sound_map_;
    std::unordered_map<std::string, int>            reserved_channels_;
    std::vector<FMOD::Channel *>                    channels_;
    int                                             num_channels_{32};
};

} // namespace cge

#endif // PLATFORM_AUDIO_ENGINE_HPP
