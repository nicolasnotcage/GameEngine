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

/**
* Geometry nodes draw based on state information contained within texture nodes. If 
* we are using a custom rectangular dimension, the geometry node determines which portion 
* of the texture to draw based on the rectangle data.
**/
void GeometryNode::draw(SceneState &scene_state)
{
    TextureNode *tex_node = scene_state.texture_node;
    if(!tex_node) return;

    SDL_FRect rect;
    if (tex_node->use_source_rect())
    {
        // Convert int-based rect to float-based FRect
        rect.x = static_cast<float>(tex_node->get_src_rect()->x);
        rect.y = static_cast<float>(tex_node->get_src_rect()->y);
        rect.w = static_cast<float>(tex_node->get_src_rect()->w);
        rect.h = static_cast<float>(tex_node->get_src_rect()->h);
    }
    else
    {
        rect.x = 0.0f;
        rect.y = 0.0f;
        rect.w = static_cast<float>(scene_state.texture_node->width());
        rect.h = static_cast<float>(scene_state.texture_node->height());
    }
    
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
