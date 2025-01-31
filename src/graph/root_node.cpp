/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "graph/root_node.hpp"

namespace cge
{

void RootNode::init(SceneState &scene_state) { init_children(scene_state); }

void RootNode::destroy()
{
    destroy_children();
    clear_children();
}

void RootNode::draw(SceneState &scene_state)
{
    SDL_RenderClear(scene_state.sdl_info->renderer);
    draw_children(scene_state);
    SDL_RenderPresent(scene_state.sdl_info->renderer);
}

} // namespace cge
