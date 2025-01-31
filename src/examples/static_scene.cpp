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
    
    SDL_SetRenderDrawColor(sdl_info->renderer, 64, 64, 80, 255);
    SDL_SetRenderDrawBlendMode(sdl_info->renderer, SDL_BLENDMODE_BLEND);
    
    uint8_t yellow[3] = {255, 255, 0};
    uint8_t purple[3] = {255, 0, 255};
    
    auto &tex_0 = root_.get_child<0>();
    auto &tex_1 = root_.get_child<1>();
    
    tex_0.set_filepath("images/box.png");
    tex_0.set_blend(true);
    tex_0.set_blend_alpha(200);
    tex_0.set_color_mods(yellow);
    
    tex_1.set_filepath("images/circle.png");
    tex_1.set_blend(true);
    tex_1.set_blend_alpha(200);
    tex_1.set_color_mods(purple);

    auto &geo_0 = tex_0.get_child<0>();    
    geo_0.set_top_left(100.0f, 100.0f);
    geo_0.set_top_right(300.0f, 100.0f);
    geo_0.set_bottom_left(100.0f, 300.0f);
    
    auto &geo_1 = tex_1.get_child<0>();    
    geo_1.set_top_left(400.0f, 100.0f);
    geo_1.set_top_right(600.0f, 100.0f);
    geo_1.set_bottom_left(400.0f, 300.0f);

    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;
    root_.init(scene_state_);
}

void StaticScene::destroy()
{
    root_.destroy();
}

void StaticScene::render()
{
    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;
    
    root_.draw(scene_state_);
}

} // namespace cge
