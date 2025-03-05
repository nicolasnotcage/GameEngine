#include "platform/animation.hpp"

namespace cge
{

Animation::Animation() : name_(""), looping_(false) {}

Animation::Animation(const std::string &name, bool looping) 
	: name_(name), looping_(looping) 
	{}

void Animation::add_frame(uint32_t frame_id, uint32_t duration) 
{
    frames_.push_back({frame_id, duration});
}

const std::string &Animation::get_name() const { return name_; }

void Animation::set_looping(bool looping) { looping_ = looping; }

bool Animation::is_looping() const { return looping_; }

const std::vector<cge::Animation::AnimationFrame> &Animation::get_frames() const { return frames_; }

Animator::Animator() :
    current_animation_(""),
    current_frame_index_(0),
    time_accumulator_(0.0f),
    is_playing_(false),
    playback_speed_(1.0f)
{
}

void Animator::update(double delta)
{
    if(!is_playing_ || current_animation_.empty()) return;

    auto it = animations.find(current_animation_);
    if(it == animations.end()) return;

    const Animation                              &current_anim = it->second;
    const std::vector<Animation::AnimationFrame> &frames = current_anim.get_frames();

    if(frames.empty()) return;

    // Accumulate time
    time_accumulator_ += static_cast<float>(delta * playback_speed_);

    // Check if we need to advance frames
    uint32_t current_duration = frames[current_frame_index_].duration;

    // Convert from seconds to animation ticks (assuming 60 ticks per second)
    float ticks_per_second = 60.0f;
    float time_per_tick = 1.0f / ticks_per_second;

    while(time_accumulator_ >= current_duration * time_per_tick)
    {
        // Subtract the time for this frame
        time_accumulator_ -= current_duration * time_per_tick;

        // Move to next frame
        current_frame_index_++;

        // Check if animation is complete
        if(current_frame_index_ >= frames.size())
        {
            if(current_anim.is_looping())
            {
                // Loop back to beginning
                current_frame_index_ = 0;
            }
            else
            {
                // Stop at last frame
                current_frame_index_ = frames.size() - 1;
                is_playing_ = false;
                break;
            }
        }

        // Update duration for the new frame
        current_duration = frames[current_frame_index_].duration;
    }
}

void Animator::play(const std::string &animation_name)
{
    auto it = animations.find(animation_name);
    if(it == animations.end()) return;

    // If already playing this animation, do nothing
    if(is_playing_ && current_animation_ == animation_name) return;

    current_animation_ = animation_name;
    current_frame_index_ = 0;
    time_accumulator_ = 0.0f;
    is_playing_ = true;
}

void Animator::pause() { is_playing_ = false; }

void Animator::resume() { is_playing_ = true; }

void Animator::reset()
{
    current_frame_index_ = 0;
    time_accumulator_ = 0.0f;
}

void Animator::add_animation(const Animation &animation)
{
    animations[animation.get_name()] = animation;
}

uint32_t Animator::get_current_frame_id() const
{
    if(current_animation_.empty()) return 0;

    auto it = animations.find(current_animation_);
    if(it == animations.end() || it->second.get_frames().empty()) return 0;

    const std::vector<Animation::AnimationFrame> &frames = it->second.get_frames();

    if(current_frame_index_ >= frames.size()) return 0;

    return frames[current_frame_index_].frame_id;
}

void Animator::set_playback_speed(float speed) { playback_speed_ = speed; }

void Animator::set_looping(bool looping)
{
    if(current_animation_.empty()) return;

    auto it = animations.find(current_animation_);
    if(it != animations.end()) static_cast<Animation &>(it->second).set_looping(looping);
}

} // namespace cge

