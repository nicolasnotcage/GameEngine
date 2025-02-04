/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "graph/geometry_node.hpp"

#include "graph/texture_node.hpp"
#include "graph/sprite_node.hpp"

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
    // Get current texture node
    TextureNode *tex_node = scene_state.texture_node;
    if(!tex_node) return;

    // Compute the source rectangle (always with positive dimensions).
    SDL_FRect srcFRect;
    if(tex_node->use_source_rect())
    {
        // Convert the integer-based src rect to a float-based FRect.
        srcFRect.x = static_cast<float>(tex_node->get_src_rect()->x);
        srcFRect.y = static_cast<float>(tex_node->get_src_rect()->y);
        srcFRect.w = static_cast<float>(tex_node->get_src_rect()->w);
        srcFRect.h = static_cast<float>(tex_node->get_src_rect()->h);
    }
    else
    {
        // Use the full texture.
        srcFRect.x = 0.0f;
        srcFRect.y = 0.0f;
        srcFRect.w = static_cast<float>(tex_node->width());
        srcFRect.h = static_cast<float>(tex_node->height());
    }

    // Copy the destination corners defined in this GeometryNode.
    // These corners are assumed to be set by the sprite (or client) code.
    SDL_FPoint corners[3];
    corners[0] = corners_[0];
    corners[1] = corners_[1];
    corners[2] = corners_[2];

    // Instead of negating the source rectangle, adjust the destination geometry:
    // If flipping horizontally, swap the top-left and top-right corners.
    if(tex_node->flip_horizontal()) { std::swap(corners[0], corners[1]); }
    // If flipping vertically, swap the top-left and bottom-left corners.
    if(tex_node->flip_vertical()) { std::swap(corners[0], corners[2]); }

    // Now call the SDL function to render the texture using an affine transform.
    SDL_RenderTextureAffine(scene_state.sdl_info->renderer,
                            tex_node->sdl_texture(),
                            &srcFRect,    // the (positive) source rectangle
                            &corners[0],  // top-left corner
                            &corners[1],  // top-right corner
                            &corners[2]); // bottom-left corner
}

void GeometryNode::set_top_left(float x, float y) { corners_[0] = {x, y}; }

void GeometryNode::set_top_right(float x, float y) { corners_[1] = {x, y}; }

void GeometryNode::set_bottom_left(float x, float y) { corners_[2] = {x, y}; }

} // namespace cge
