/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/audio_engine.hpp"
#include <iostream>

namespace cge
{

AudioEngine* AudioEngine::get_instance()
{ 
	static AudioEngine instance;
    return &instance;
}

AudioEngine::AudioEngine() = default;
AudioEngine::~AudioEngine() { this->shutdown(); }

// Initialize the FMOD system. Returns true if successful, false otherwise. 
bool AudioEngine::init(int max_channels, bool use_3d)
{
	// Create FMOD system
    FMOD_RESULT result = FMOD::System_Create(&fmod_system_);
    if (result != FMOD_OK) 
    { 
        std::cerr << "Failed to create FMOD system...\n";
        return false;
    }

    // Get version
    unsigned int version;
    fmod_system_->getVersion(&version);
    std::cout << "FMOD Version: " 
              << ((version >> 16) & 0xFF) << "." 
              << ((version >> 8) & 0xFF)  << "." 
              << (version & 0xFF) << "\n";

    // Setup
    num_channels_ = max_channels;
    fmod_system_->getSoftwareChannels(&num_channels_);
    channels_.resize(num_channels_, nullptr);

    // Initialize with flags
    FMOD_INITFLAGS flags = FMOD_INIT_NORMAL;
    if (use_3d) flags |= FMOD_INIT_3D_RIGHTHANDED;

    result = fmod_system_->init(num_channels_, flags, nullptr);
    if(result != FMOD_OK)
    {
        std::cerr << "Failed to initialize FMOD system\n";
        return false;
    }

    return true;
}

// Shutdown the FMOD system. 
void AudioEngine::shutdown()
{
    // Release all sounds
    for(auto &[key, sound] : sound_map_)
    {
        if(sound) sound->release();
    }

    sound_map_.clear();

    // Release FMOD system
    if(fmod_system_)
    {
        fmod_system_->close();
        fmod_system_->release();
        fmod_system_ = nullptr;
    }

    channels_.clear();
}

// Load a sound into the FMOD system.  
bool AudioEngine::load_sound(const std::string &path,
                             const std::string &key,
                             bool               is_3d,
                             bool               looping)
{
    // Skip if already loaded
    if(sound_map_.find(key) != sound_map_.end()) return true;

    // Set mode
    FMOD_MODE mode = is_3d ? FMOD_3D : FMOD_2D;
    if(looping) { mode |= FMOD_LOOP_NORMAL; }
    else { mode |= FMOD_LOOP_OFF; }

    // Load sound
    FMOD::Sound *sound = nullptr;
    FMOD_RESULT  result = fmod_system_->createSound(path.c_str(), mode, nullptr, &sound);

    if(result != FMOD_OK)
    {
        std::cerr << "Failed to load sound: " << path << std::endl;
        return false;
    }

    // Store sound in map
    sound_map_[key] = sound;
    return true;
}

// Unload a sound from the sound map by key. 
// TODO: Should notify user if bad key passed?
void AudioEngine::unload_sound(const std::string &key)
{
    auto it = sound_map_.find(key);
    if(it != sound_map_.end())
    {
        if(it->second) it->second->release();
        sound_map_.erase(it);
    }
}

// Get sound object by key. 
FMOD::Sound* AudioEngine::get_sound(const std::string &key)
{
    auto it = sound_map_.find(key);
    if(it != sound_map_.end()) { return it->second; }
    return nullptr;
}

// Play sound by key and return the channel id. Takes a boolean argument representing whether the sound 
// should be played immediately or paused, which allows the caller to determine when the audio is played.
// This is useful when callers intend to add DSP effects to the audio. 
// 
// TODO: Break this into separate play() and prepare() functions?
int AudioEngine::play_sound(const std::string &key, float volume, bool pause)
{
    FMOD::Sound *sound = this->get_sound(key);
    if(!sound) return -1;

    // Check if sound has a reserved channel
    int  channel_id = -1;
    auto it = reserved_channels_.find(key);
    if(it != reserved_channels_.end()) 
    { 
        channel_id = it->second; 
    }
    else
    { 
        channel_id = get_next_available_channel(); 
    }

    if(channel_id >= num_channels_) return -1;

    // Stop any sound playing on this channel
    if(channels_[channel_id]) channels_[channel_id]->stop();

    // For debugging
    std::cout << "Total Number of Channels: " << num_channels_ << "\n";
    std::cout << "Channel ID Assigned: " << channel_id << "\n";

    // Configure sound
    FMOD_RESULT result = fmod_system_->playSound(sound, nullptr, pause, &channels_[channel_id]);
    if(result != FMOD_OK) return -1;

    // Set volume
    channels_[channel_id]->setVolume(volume);

    return channel_id;
}

// Creates echo effect and adds it to the passed channel. 
void AudioEngine::add_echo(int channel_id, float delay_ms, float feedback)
{
    if(channel_id < 0 || channel_id >= num_channels_ || !channels_[channel_id]) return;

    // Create effect
    FMOD::DSP *dsp = nullptr;
    fmod_system_->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);

    // Set parameters
    dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, delay_ms);
    dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, feedback);

    // Add to channel
    channels_[channel_id]->addDSP(0, dsp);
}

// Update the FMOD system and clean up unused channels.
// TODO: Is channel cleanup already handled by FMOD system?
void AudioEngine::update()
{
    if(fmod_system_) 
    { 
        fmod_system_->update();
        update_channel_statuses();
    }
}

// Returns the next available channel id. 
int AudioEngine::get_next_available_channel()
{
    // Check for nullptr channels
    for(int i = 0; i < num_channels_; i++)
    {
        if(channels_[i] == nullptr) return i;
    }

    // No available channels
    return num_channels_;
}

// Returns the channel object associated with a channel id.
FMOD::Channel* AudioEngine::get_channel(int channel_id)
{
    if(channel_id >= 0 && channel_id < channels_.size()) { return channels_[channel_id]; }
    return nullptr;
}

// Clean up unused channels. 
void AudioEngine::update_channel_statuses()
{
    for(int i = 0; i < num_channels_; i++)
    {
        if(channels_[i] != nullptr)
        {
            bool        is_playing = false;
            FMOD_RESULT result = channels_[i]->isPlaying(&is_playing);

            // If channel is not playing or we couldn't determine state, clear it
            if(result != FMOD_OK || !is_playing) { channels_[i] = nullptr; }
        }
    }
}

// Reserves a channel id for the given sound key. 
void AudioEngine::reserve_channel_for_sound(const std::string &sound_key, int channel_id)
{
    if(channel_id >= 0 && channel_id < num_channels_)
    {
        reserved_channels_[sound_key] = channel_id;
    }
}

// TODO: Get this working so we can have directional audio
void AudioEngine::set_3d_listener_position(const Vector2& position)
{
    if(!fmod_system_) return;

    // Convert to FMOD vector
    FMOD_VECTOR pos = {position.x, position.y, 0.0f};
    
    // Set up direction vector
    FMOD_VECTOR forward = {0.0f, 1.0f, 0.0f};
    FMOD_VECTOR up = {0.0f, 0.0f, 1.0f};

    // TODO: Still unsure of how to think of velocity in the context of the game and the audio.
    FMOD_VECTOR velocity = {0.0f, 0.0f, 0.0f};

    // Update listener position
    fmod_system_->set3DListenerAttributes(0, &pos, &velocity, &forward, &up);
}

} // namespace cge
