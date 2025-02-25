/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "graph/geometry_node.hpp"

#include "graph/texture_node.hpp"
#include "graph/sprite_node.hpp"
#include "platform/math.hpp"
#include "platform/config.hpp"
#include "graph/camera_node.hpp"

#include <iostream>

namespace cge
{

void GeometryNode::init(SceneState &scene_state) {}

void GeometryNode::destroy() { clear_children(); }

void GeometryNode::draw(SceneState &scene_state)
{
    SDL_FRect rect{};
    rect.x = 0.0f;
    rect.w = static_cast<float>(scene_state.texture_node->width());
    rect.y = 0.0f;
    rect.h = static_cast<float>(scene_state.texture_node->height());

    // Get vertices in world space using the current transformation matrix
    auto tl_world = scene_state.matrix_stack.top() * Vector2(-0.5f, -0.5f);
    auto tr_world = scene_state.matrix_stack.top() * Vector2(0.5f, -0.5f);
    auto bl_world = scene_state.matrix_stack.top() * Vector2(-0.5f, 0.5f);

     // Convert from world space to screen space using the camera
    auto tl_screen = scene_state.active_camera->get_camera().world_to_screen(tl_world, 
        cge::SCREEN_WIDTH, cge::SCREEN_HEIGHT);
    auto tr_screen = scene_state.active_camera->get_camera().world_to_screen(
        tr_world, cge::SCREEN_WIDTH, cge::SCREEN_HEIGHT);
    auto bl_screen = scene_state.active_camera->get_camera().world_to_screen(
        bl_world, cge::SCREEN_WIDTH, cge::SCREEN_HEIGHT);


     // Use the screen space coordinates for rendering
    SDL_FPoint top_left{tl_screen.x, tl_screen.y};
    SDL_FPoint top_right{tr_screen.x, tr_screen.y};
    SDL_FPoint bottom_left{bl_screen.x, bl_screen.y};

    SDL_RenderTextureAffine(scene_state.sdl_info->renderer,
                            scene_state.texture_node->sdl_texture(),
                            &rect,
                            &top_left,
                            &top_right,
                            &bottom_left);
}

void GeometryNode::update(SceneState& scene_state)
{
    // TODO: Do something.
}

} // namespace cge
