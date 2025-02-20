/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/static_scene.hpp"

namespace cge
{

void StaticScene::init(SDLInfo *sdl_info)
{
    sdl_info_ = sdl_info;

    SDL_SetRenderDrawColor(sdl_info->renderer, 28, 40, 51, 0);
    SDL_SetRenderDrawBlendMode(sdl_info->renderer, SDL_BLENDMODE_BLEND);

    auto &transform_0 = root_.get_child<0>();
    auto &tex_0 = transform_0.get_child<0>();
    auto &geo_0 = tex_0.get_child<0>();

    auto &transform_1 = root_.get_child<1>();
    auto &tex_1 = transform_1.get_child<0>();
    auto &geo_1 = tex_1.get_child<0>();

    // Set transform nodes to identity matrix
    transform_0.set_identity();
    transform_1.set_identity();

    // Configure data of children nodes using wrapper functions
    tex_0.set_filepath("images/fireplace.png");
    tex_0.set_blend(true);
    tex_0.set_blend_alpha(200);

    tex_1.set_filepath("images/witch_run.png");
    tex_1.set_blend(true);
    tex_1.set_blend_alpha(200);

    // Reset SDLInfo and texture node to nullptr within the scene state struct
    scene_state_.reset();

    // Set scene state SDLInfo data to new SDLInfo data
    scene_state_.sdl_info = sdl_info_;

    // Initialize root node with new scene state (texture node is still null)
    root_.init(scene_state_);
}

void StaticScene::destroy() { root_.destroy(); }

void StaticScene::render()
{
    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;

    root_.draw(scene_state_);
}

void StaticScene::update(double delta) 
{ 
    scene_state_.delta = delta;
    root_.update(scene_state_); 
}

} // namespace cge
