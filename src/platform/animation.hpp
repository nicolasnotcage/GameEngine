/**
* This file contains definitions for both Animation and Animator classes, which 
* define the animation behavior used within the engine.
**/

#ifndef PLATFORM_ANIMATION
#define PLATFORM_ANIMATION

#include <vector>
#include <string>
#include <unordered_map>

namespace cge
{

struct Frame
{
    int x, y;           // Position in sprite sheet
    int width, height;  // Dimensions
};

// Contains data specific to an animation (i.e., frames with timing information).
class Animation
{
public:
    struct AnimationFrame
    {
        uint32_t frame_id; // position in sprite sheet
        uint32_t duration; // in ticks
    };

    Animation();
    Animation(const std::string &name, bool looping);

    void add_frame(uint32_t frame_id, uint32_t duration);
    const std::string &get_name() const;
    void set_looping(bool looping);
    bool is_looping() const;
    const std::vector<AnimationFrame> &get_frames() const;
    

private:
    std::string name_;
    std::vector<AnimationFrame> frames_;
    bool looping_;
};

// Animator class controls playback of animations
class Animator
{
public:
    Animator();

    void update(double delta);
    void play(const std::string &animation_name);
    void pause();
    void resume();
    void reset();

    void add_animation(const Animation &animation);
    uint32_t get_current_frame_id() const;
    const std::string &get_current_animation_name() const;
    bool is_playing() const;


    // Timeline control
    void set_playback_speed(float speed); 
    void set_looping(bool looping);

private:
    std::unordered_map<std::string, Animation> animations;
    std::string current_animation_;
    uint32_t current_frame_index_;
    float time_accumulator_;
    bool is_playing_;
    float playback_speed_;

};

} // namespace cge

#endif // PLATFORM_ANIMATION
