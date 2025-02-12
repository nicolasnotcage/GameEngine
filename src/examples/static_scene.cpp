/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/static_scene.hpp"

namespace cge
{

/**
 * This scene contains a demonstration of new sprite, texture, and geometry functionality.
 * Within the scene, we load in two sprite sheets as textures. We set custom rectangles that
 * are used to focus on specific images within each sprite sheet.
 *
 * Additionally, we horizontally and vertically flip the second sprite. Flipping is still a little
 * buggy, as the vertical flip offset is supplemented with a simple transform in geometry_node.cpp
 * to "center" the image after the flip. I'm sure there's a better way to do it, but this works for
 * now.
 **/
void StaticScene::init(SDLInfo *sdl_info)
{
    sdl_info_ = sdl_info;

    SDL_SetRenderDrawColor(sdl_info->renderer, 28, 40, 51, 0);
    SDL_SetRenderDrawBlendMode(sdl_info->renderer, SDL_BLENDMODE_BLEND);

    auto &sprite_0 = root_.get_child<0>();
    auto &tex_0 = sprite_0.get_child<0>();
    auto &geo_0 = tex_0.get_child<0>();

    auto &sprite_1 = root_.get_child<1>();
    auto &tex_1 = sprite_1.get_child<0>();
    auto &geo_1 = tex_1.get_child<0>();

    // Configure data of children nodes using wrapper functions
    tex_0.set_filepath("images/fireplace.png");
    tex_0.set_blend(true);
    tex_0.set_blend_alpha(200);

    tex_1.set_filepath("images/witch_run.png");
    tex_1.set_blend(true);
    tex_1.set_blend_alpha(200);

    // Set geometry locations
    geo_0.set_top_left(230, 150.0f);
    geo_0.set_top_right(394, 150.0f);
    geo_0.set_bottom_left(230, 314.0f);

    geo_1.set_top_left(380.0f, 150.0f);
    geo_1.set_top_right(544.0f, 150.0f);
    geo_1.set_bottom_left(380.0f, 314.0f);

    /**
     * Set custom sprite sheet info (helpful to use Godot sprite sheet tool to get these values).
     *
     * Variable overview:
     * - Frame index: The frame that should be displayed (goal: place frame indices in sprite nodes
     *to support animation)
     * - Frame width: Width of the sprite sheet frame
     * - Frame height: Height of the sprite sheet frame
     * - x_offset (y_offset): The location of the start of the sprite. Can be x or y depending on
     *the sprite sheet.
     *
     * TODO: Place these into a dedicated class. Part of the sprite node class?
     *
     **/
    int tex_0_frame_index = 0;
    int tex_0_frame_width = 64;
    int tex_0_frame_height = 64;
    int tex_0_x_offset = tex_0_frame_index * tex_0_frame_width;

    // Set data within texture node
    tex_0.set_source_rect(tex_0_x_offset, 0, tex_0_frame_width, tex_0_frame_height);

    /**
     * Sprite 1 frame info (note that this one uses a y-offset)
     **/
    int tex_1_frame_index = 0;
    int tex_1_frame_width = 64;
    int tex_1_frame_height = 64;
    int tex_1_y_offset = tex_1_frame_index * tex_1_frame_width;

    tex_1.set_source_rect(0, tex_1_y_offset, tex_1_frame_width, tex_1_frame_height);

    // Flip sprite 1 for testing
    tex_1.set_flip_horizontal(true);
    tex_1.set_flip_vertical(true);

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

} // namespace cge
