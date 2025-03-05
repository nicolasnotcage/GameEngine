/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/static_scene.hpp"
#include "platform/animation.hpp"

namespace cge
{

void StaticScene::init(SDLInfo *sdl_info, IoHandler *io_handler)
{
    sdl_info_ = sdl_info;
    io_handler_ = io_handler;

    SDL_SetRenderDrawColor(sdl_info->renderer, 28, 40, 51, 0);
    SDL_SetRenderDrawBlendMode(sdl_info->renderer, SDL_BLENDMODE_BLEND);

    // Camera
    auto &camera = root_.get_child<0>();

    // Set camera dimensions
    camera.get_camera().set_dimensions(20.0f, 15.0f);

    // Position camera to look at origin initially
    camera.get_camera().set_position(0.0f, 0.0f);

    // Configure Golem
    auto &golem_transform = camera.get_child<0>();
    auto &golem_tex = golem_transform.get_child<0>();
    auto &golem_geo = golem_tex.get_child<0>();
    
    // Golem Transforms
    golem_transform.right_translate(3.0f, 5.0f);
    golem_transform.right_scale(3.0f, 3.0f);

    // Golem Textures
    golem_tex.set_filepath("images/golem_walk.png");
    golem_tex.set_blend(true);
    golem_tex.set_blend_alpha(200);

    // Golem SpriteSheet and Animations
    uint32_t golem_frames = 7;
    golem_tex.define_grid(golem_frames, 1, 64, 64);
    golem_tex.create_animator();
    Animation walk_animation("walk", true);

    // Add a frame to the animation for each frame in the sprite sheet
    for(int i = 0; i < golem_frames; i++) { walk_animation.add_frame(i, 10); }

    // Add animation to animator
    golem_tex.get_animator().add_animation(walk_animation);

    // Start playing animation (TODO: Make this animation only play while a directional key is pressed)
    golem_tex.get_animator().play("walk");
    
    // Configure Witch
    auto &witch_transform = camera.get_child<1>();
    auto &witch_tex = witch_transform.get_child<0>();
    auto &witch_geo = witch_tex.get_child<0>();

    // Witch Transforms
    witch_transform.right_translate(3.0f, 10.0f);
    witch_transform.right_scale(3.0f, 3.0f);
    
    // Witch Textures
    witch_tex.set_filepath("images/witch_run.png");
    witch_tex.set_blend(true);
    witch_tex.set_blend_alpha(200);

    // Witch SpriteSheet and Animations
    uint32_t witch_frames = 6;
    witch_tex.define_grid(1, witch_frames, 64, 64);
    witch_tex.create_animator();
    Animation witch_walk("walk", true);

    // Add a frame to the animation for each frame in the sprite sheet
    for(int i = 0; i < witch_frames; i++) { witch_walk.add_frame(i, 10); }

    // Add animation to animator
    witch_tex.get_animator().add_animation(witch_walk);

    // Start playing animation (TODO: Automate this to walk from point to point)
    witch_tex.get_animator().play("walk");

    // Reset SDLInfo and texture node to nullptr within the scene state struct
    scene_state_.reset();

    // Set scene state SDLInfo data to new SDLInfo data
    scene_state_.sdl_info = sdl_info_;

    // Configure io handler
    scene_state_.io_handler = io_handler_;

    // Initialize root node with new scene state
    root_.init(scene_state_);
}

void StaticScene::destroy() { root_.destroy(); }

void StaticScene::render()
{
    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;
    scene_state_.io_handler = io_handler_;

    root_.draw(scene_state_);
}

void StaticScene::update(double delta)
{
    scene_state_.io_handler = io_handler_;
    scene_state_.delta = delta;

    root_.update(scene_state_);
}

} // namespace cge
