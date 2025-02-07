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
* Geometry nodes draw based on state information contained within texture nodes. If we
* are using a custom rectangular dimension (e.g., when using sprite sheets), the geometry node
* determines which portion of the texture to draw using data stored in the active texture node.
* 
* Two rectangles in the draw function:
*     1) Source Rectangle: Defines "what" will be drawn (the whole texture or part of it, as defined in the texture node)
*     2) Destination Rectangle: Defines "where" the texture will be drawn
**/
void GeometryNode::draw(SceneState &scene_state)
{
    // Get current texture node
    TextureNode *tex_node = scene_state.texture_node;
    if(!tex_node) return;

    // Compute the source rectangle (always with positive dimensions). 
    // Is either the whole texture or part of it (when using sprite sheets)
    SDL_FRect src_rect;
    if(tex_node->use_source_rect()) 
    { 
        // Use the dimensions defined in the texture node. 
        // Convert the integer-based src rect to a float-based FRect.
        src_rect.x = static_cast<float>(tex_node->get_src_rect()->x);
        src_rect.y = static_cast<float>(tex_node->get_src_rect()->y);
        src_rect.w = static_cast<float>(tex_node->get_src_rect()->w);
        src_rect.h = static_cast<float>(tex_node->get_src_rect()->h);
    }
    else
    {
        // Use the full texture.
        src_rect.x = 0.0f;
        src_rect.y = 0.0f;
        src_rect.w = tex_node->width();
        src_rect.h = tex_node->height();
    }

    // Compute the destination rectangle based on the corners. (x, y) define the top-left corner. Width and height
    // are calculated using the distance between the corners. 
    // TODO: Error handling (what if corners yield negative values?)
    SDL_FRect dst_rect;
    dst_rect.x = corners_[0].x;
    dst_rect.y = corners_[0].y;
    dst_rect.w = corners_[1].x - corners_[0].x;
    dst_rect.h = corners_[2].y - corners_[0].y;

    // Compensates for the y-axis shift caused by the flip, but it's still not 100% right
    // TODO: Find a more mathematically precise solution
    if(tex_node->flip_vertical())
    {
        dst_rect.y += (dst_rect.h / 2);
    }

    // Determine and set the flip flags
    SDL_FlipMode flip = SDL_FLIP_NONE;
    if (tex_node->flip_horizontal())
    {
        flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_HORIZONTAL);
    }
    if (tex_node->flip_vertical()) 
    { 
        flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_VERTICAL);
    }

    // Render texture with flipping
    SDL_RenderTextureRotated(scene_state.sdl_info->renderer,
                             tex_node->sdl_texture(),
                             &src_rect,
                             &dst_rect,
                             0,
                             nullptr,
                             flip);
}

void GeometryNode::update(SceneState& scene_state)
{
    // TODO: Do something.
}

void GeometryNode::set_top_left(float x, float y) { corners_[0] = {x, y}; }

void GeometryNode::set_top_right(float x, float y) { corners_[1] = {x, y}; }

void GeometryNode::set_bottom_left(float x, float y) { corners_[2] = {x, y}; }

} // namespace cge
