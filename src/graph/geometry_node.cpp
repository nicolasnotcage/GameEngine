/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "graph/geometry_node.hpp"

#include "graph/texture_node.hpp"

#include <iostream>

namespace cge
{

void GeometryNode::init(SceneState &scene_state) {}

void GeometryNode::destroy() { clear_children(); }

void GeometryNode::draw(SceneState &scene_state)
{
    SDL_FRect rect;
    rect.x = 0.0f;
    rect.w = static_cast<float>(scene_state.texture_node->width());
    rect.y = 0.0f;
    rect.h = static_cast<float>(scene_state.texture_node->height());

    SDL_RenderTextureAffine(scene_state.sdl_info->renderer,
                            scene_state.texture_node->sdl_texture(),
                            &rect,
                            &corners_[0],
                            &corners_[1],
                            &corners_[2]);
}

void GeometryNode::set_top_left(float x, float y) { corners_[0] = {x, y}; }

void GeometryNode::set_top_right(float x, float y) { corners_[1] = {x, y}; }

void GeometryNode::set_bottom_left(float x, float y) { corners_[2] = {x, y}; }

} // namespace cge
