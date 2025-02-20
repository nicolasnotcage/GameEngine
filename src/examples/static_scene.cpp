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

    // Parent sprite (Sphere)
    auto &parent = root_.get_child<0>();
    auto &parent_tex = parent.get_child<0>();
    auto &parent_geo = parent_tex.get_child<0>();

    // First child sprite (MolePerson) - relative to parent
    auto &child1 = parent.get_child<1>();
    auto &child1_tex = child1.get_child<0>();
    auto &child1_geo = child1_tex.get_child<0>();

    // Second child sprite (Player) - relative to parent
    auto &child2 = parent.get_child<2>();
    auto &child2_tex = child2.get_child<0>();
    auto &child2_geo = child2_tex.get_child<0>();

    // Parent Transform
    parent.right_translate(400, 200);
    parent.right_scale(300, 300);

    // Child Transforms
    //     Must account for the scaling of the parent so we divide the transform values 
    //     by 300 before applying them.

    // Place child 1 200 pixels to the left of the parent and 100 pixels up. 
    // Also rotate it 315 to give a 45 degree left rotation.
    child1.right_translate(-200.0f / 300.0f, -100.0f / 300.0f);
    child1.right_rotate_degrees(315);
    child1.right_scale(0.5f, 0.5f);

    // Place child 2 200 pixels to the right of the parent and 100 pixels up. 
    // Also rotate it 45 degrees to the right.
    child2.right_translate(200.0f / 300.0f, -100.0f / 300.0f);
    child2.right_rotate_degrees(45);
    child2.right_scale(0.5f, 0.5f);

    // Configure textures
    parent_tex.set_filepath("images/Sphere.png");
    parent_tex.set_blend(true);
    parent_tex.set_blend_alpha(200);

    child1_tex.set_filepath("images/MolePerson.png");
    child1_tex.set_blend(true);
    child1_tex.set_blend_alpha(200);

    child2_tex.set_filepath("images/Player.png");
    child2_tex.set_blend(true);
    child2_tex.set_blend_alpha(200);

    // Reset SDLInfo and texture node to nullptr within the scene state struct
    scene_state_.reset();

    // Set scene state SDLInfo data to new SDLInfo data
    scene_state_.sdl_info = sdl_info_;

    // Initialize root node with new scene state
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
