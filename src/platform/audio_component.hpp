#ifndef PLATFORM_AUDIO_COMPONENT_HPP
#define PLATFORM_AUDIO_COMPONENT_HPP

#include "platform/math.hpp"
#include <string>

namespace cge
{

class TransformNode;

// A component class that represents a singular instance of audio
// that is attached to a game object. 
class AudioComponent
{
public:
    AudioComponent(TransformNode *owner);
    ~AudioComponent();

    // Sound playback
    void set_sound(const std::string &sound_key);
    int  play(float volume = 1.0f);
    void stop();
    void pause();
    void resume();

    // Sound Control
    void set_volume(float volume);
    void set_pitch(float pitch);
    void set_loop(bool loop);

    // 3D Positioning
    void set_min_distance(float min_distance);
    void set_max_distance(float max_distance);
    void update_position();

    // DSP
    void set_echo(bool enabled, float delay_ms = 300.0f, float feedback = 30.0f);

    // State
    bool is_playing() const;
    TransformNode *get_owner() const { return owner_; }

private:
    TransformNode *owner_;
    std::string    sound_key_;
    int            channel_id_{-1};
    float          volume_{1.0f};
    float          pitch_{1.0f};
    bool           loop_{false};
    float          min_distance_{1.0f};
    float          max_distance_{10000.0f};
    bool           is_3d_{false};
    bool           has_echo_{false};
    float          echo_delay_{0.0f};
    float          echo_feedback_{0.0f};
};

} // namespace cge

#endif // PLATFORM_AUDIO_COMPONENT_HPP
