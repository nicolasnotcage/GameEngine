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

    // Reset scene state
    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;
    scene_state_.io_handler = io_handler_;

    // Initialize all textures

    // Golem walk texture
    golem_walk_texture_.set_filepath("images/golem_walk.png");
    golem_walk_texture_.set_blend(true);
    golem_walk_texture_.set_blend_alpha(200);
    golem_walk_texture_.define_grid(7, 1, 64, 64);
    golem_walk_texture_.init(scene_state_);

    // Golem idle texture
    golem_idle_texture_.set_filepath("images/golem_idle.png");
    golem_idle_texture_.set_blend(true);
    golem_idle_texture_.set_blend_alpha(200);
    golem_idle_texture_.define_grid(12, 1, 64, 64);
    golem_idle_texture_.init(scene_state_);

    // Witch run texture
    witch_run_texture_.set_filepath("images/witch_run.png");
    witch_run_texture_.set_blend(true);
    witch_run_texture_.set_blend_alpha(200);
    witch_run_texture_.define_grid(1, 6, 64, 64);
    witch_run_texture_.init(scene_state_);

    // Camera setup
    auto &camera = root_.get_child<0>();
    camera.get_camera().set_dimensions(20.0f, 15.0f);
    camera.get_camera().set_position(0.0f, 0.0f);

    // Configure transforms
    auto &golem_transform = camera.get_child<0>();
    auto &witch_transform = camera.get_child<1>();

    // Position golem and witch
    golem_transform.right_translate(1.0f, 0.0f);
    golem_transform.right_scale(3.0f, 3.0f);

    witch_transform.right_translate(1.0f, 4.0f);
    witch_transform.right_scale(3.0f, 3.0f);

    // Setup animations for both characters
    setup_golem_animations();
    setup_witch_animations();

    // Set camera to follow golem
    camera.set_target(&golem_transform, true);
    camera.set_follow_smoothness(1.0f);

    // Initialize root node
    root_.init(scene_state_);
}

void StaticScene::setup_golem_animations()
{
    auto &camera = root_.get_child<0>();
    auto &golem_transform = camera.get_child<0>();
    auto &golem_sprite = golem_transform.get_child<0>();

    // Create walk animation
    Animation walk_animation("walk", true);
    for(int i = 0; i < 7; i++) { walk_animation.add_frame(i, 10); }

    // Create idle animation
    Animation idle_animation("idle", true);
    for(int i = 0; i < 12; i++) { idle_animation.add_frame(i, 10); }

    // Add animations with their respective textures
    golem_sprite.add_animation_with_texture(walk_animation, &golem_walk_texture_);
    golem_sprite.add_animation_with_texture(idle_animation, &golem_idle_texture_);

    // Set initial texture
    golem_sprite.set_texture(&golem_idle_texture_);

    // Start playing idle animation by default
    golem_sprite.play("idle");

    // Set golem as player-controlled
    golem_sprite.set_player_controlled(golem_transform);
}

void StaticScene::setup_witch_animations()
{
    auto &camera = root_.get_child<0>();
    auto &witch_transform = camera.get_child<1>();
    auto &witch_sprite = witch_transform.get_child<0>();

    // Create run animation
    Animation walk_animation("walk", true);
    for(int i = 0; i < 6; i++) { walk_animation.add_frame(i, 10); }

    // Add animations with their respective textures
    witch_sprite.add_animation_with_texture(walk_animation, &witch_run_texture_);

    // Set initial texture
    witch_sprite.set_texture(&witch_run_texture_);

    // Start playing run animation by default
    witch_sprite.play("run");

    // Setup paths for automated movement
    witch_path_.add_point(3.0f, 10.0f, 1.0f); // Start position with 1 sec pause
    witch_path_.add_point(8.0f, 10.0f, 0.5f); // Move right
    witch_path_.add_point(8.0f, 5.0f, 0.5f);  // Move up
    witch_path_.add_point(3.0f, 5.0f, 0.5f);  // Move left
    witch_path_.add_point(3.0f, 10.0f, 1.0f); // Back to start
    witch_path_.set_looping(true);

    // Set witch as path controlled
    witch_sprite.set_path_controlled(witch_transform, witch_path_);
}

void StaticScene::destroy()
{
    root_.destroy();

    // Explicitly destroy texture nodes
    golem_walk_texture_.destroy();
    golem_idle_texture_.destroy();
    witch_run_texture_.destroy();
}

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
