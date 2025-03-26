/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/audio_component.hpp"
#include "platform/audio_engine.hpp"
#include "graph/transform_node.hpp"

namespace cge
{

AudioComponent::AudioComponent(TransformNode *owner) : owner_(owner) {}
AudioComponent::~AudioComponent() { this->stop(); }

// Sets the associated sound and whether that sound is 3D. 
void AudioComponent::set_sound(const std::string &sound_key)
{
    sound_key_ = sound_key;

    // Check if sound is 3D
    FMOD::Sound *sound = AudioEngine::get_instance()->get_sound(sound_key);
    if(sound) 
    { 
        FMOD_MODE mode;
        sound->getMode(&mode);
        is_3d_ = (mode & FMOD_3D) != 0;
    }
}

// Play the audio clip.
int AudioComponent::play(float volume)
{
    // Stop existing playback
    stop();

    // Prepare sound. The sound is paused when returned from the engine. 
    channel_id_ = AudioEngine::get_instance()->play_sound(sound_key_, volume, true);

    // Set 3D position if needed
    if(is_3d_ && channel_id_ >= 0) { update_position(); }

    // Apply effects
    if(has_echo_ && channel_id_ >= 0)
    {
        AudioEngine::get_instance()->add_echo(channel_id_, echo_delay_, echo_feedback_);
    }

    // Resume audio
    this->resume();

    return channel_id_;
}

// Stop the audio clip.
void AudioComponent::stop()
{
    if(channel_id_ >= 0) 
    {
        FMOD::Channel *channel = AudioEngine::get_instance()->get_channel(channel_id_);
        if(channel) channel->stop();
        channel_id_ = -1;
    }
}

// Pause the audio clip.
void AudioComponent::pause() 
{
    if(channel_id_ >= 0)
    {
        FMOD::Channel *channel = AudioEngine::get_instance()->get_channel(channel_id_);
        if(channel) channel->setPaused(true);
    }
}

// Resume the audio clip.
void AudioComponent::resume()
{
    if(channel_id_ >= 0)
    {
        FMOD::Channel *channel = AudioEngine::get_instance()->get_channel(channel_id_);
        if(channel) channel->setPaused(false);
    }
}

// Set the volume of the audio clip.
void AudioComponent::set_volume(float volume)
{
    volume_ = volume;
    if(channel_id_ >= 0)
    {
        FMOD::Channel *channel = AudioEngine::get_instance()->get_channel(channel_id_);
        if(channel) channel->setVolume(volume);
    }
}

// Set the pitch of the audio clip.
void AudioComponent::set_pitch(float pitch)
{
    pitch_ = pitch;
    if(channel_id_ >= 0)
    {
        FMOD::Channel *channel = AudioEngine::get_instance()->get_channel(channel_id_);
        if(channel) channel->setPitch(pitch);
    }
}

// Set whether the audio clip should loop.
void AudioComponent::set_loop(bool loop)
{
    loop_ = loop;
    if(channel_id_ >= 0)
    {
        FMOD::Channel *channel = AudioEngine::get_instance()->get_channel(channel_id_);
        if(channel)
        {
            FMOD_MODE mode;
            channel->getMode(&mode);

            // Clear existing loop flags
            mode &= ~(FMOD_LOOP_OFF | FMOD_LOOP_NORMAL);

            // Set new loop flag
            mode |= loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

            channel->setMode(mode);
        }
    }
}

// Configure echo effect. 
void AudioComponent::set_echo(bool enabled, float delay_ms, float feedback)
{ 
    has_echo_ = enabled;
    echo_delay_ = delay_ms;
    echo_feedback_ = feedback;
}

// Returns whether the audio clip is currently playing.
bool AudioComponent::is_playing() const
{
    if(channel_id_ < 0) return false;

    FMOD::Channel *channel = AudioEngine::get_instance()->get_channel(channel_id_);
    if(!channel) return false;

    bool playing = false;
    channel->isPlaying(&playing);
    return playing;
}

// -------------------------------------------------------
//                   3D Audio
// -------------------------------------------------------
// TODO: Need to get these working

// Minimum distance for 3D audio attenuation.
void AudioComponent::set_min_distance(float min_distance)
{
    min_distance_ = min_distance;
    if(is_3d_ && channel_id_ >= 0)
    {
        FMOD::Channel *channel = AudioEngine::get_instance()->get_channel(channel_id_);
        if(channel) channel->set3DMinMaxDistance(min_distance_, max_distance_);
    }
}

// Maximum distance for 3D audio attenuation.
void AudioComponent::set_max_distance(float max_distance)
{
    max_distance_ = max_distance;
    if(is_3d_ && channel_id_ >= 0)
    {
        FMOD::Channel *channel = AudioEngine::get_instance()->get_channel(channel_id_);
        if(channel) channel->set3DMinMaxDistance(min_distance_, max_distance_);
    }
}

// Update the 3D attributes of the current channel to track position. 
void AudioComponent::update_position()
{
    if(!is_3d_ || !owner_ || channel_id_ < 0) return;

    FMOD::Channel *channel = AudioEngine::get_instance()->get_channel(channel_id_);
    if(!channel) return;

    // Get position from transform node
    float x = owner_->get_position_x();
    float y = owner_->get_position_y();

    // Convert to FMOD vector
    FMOD_VECTOR position = {x, y, 0.0f};

    // Calculate velocity
    float prev_x = owner_->get_prev_position_x();
    float prev_y = owner_->get_prev_position_y();

    // TODO: Pass SceneState object to access delta time
    // Get delta time from scene state
    float delta_time = 1.0f / 60.0f;

    FMOD_VECTOR velocity = {(x - prev_x) / delta_time, (y - prev_y) / delta_time, 0.0f};

    // Set 3D attributes
    channel->set3DAttributes(&position, &velocity);
}

} // namespace cge
