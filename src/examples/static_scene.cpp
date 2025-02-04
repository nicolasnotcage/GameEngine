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
    // Set local pointer to passed argument
    sdl_info_ = sdl_info;
    
    // Set color and blend info for drawing
    SDL_SetRenderDrawColor(sdl_info->renderer, 28, 40, 51, 0);
    SDL_SetRenderDrawBlendMode(sdl_info->renderer, SDL_BLENDMODE_BLEND);

    // Get children of root node
    auto &sprite_0 = root_.get_child<0>();
    auto &sprite_1 = root_.get_child<1>();
    
    // Configure data of children nodes using wrapper functions
    sprite_0.set_filepath("images/fireplace.png");
    sprite_0.set_blend(true);
    sprite_0.set_blend_alpha(200);

    sprite_1.set_filepath("images/witch_run.png");
    sprite_1.set_blend(true);
    sprite_1.set_blend_alpha(200);

    // Set location
    sprite_0.set_top_left(230, 150.0f);
    sprite_0.set_top_right(394, 150.0f);
    sprite_0.set_bottom_left(230, 314.0f);

    sprite_1.set_top_left(380.0f, 150.0f);
    sprite_1.set_top_right(544.0f, 150.0f);
    sprite_1.set_bottom_left(380.0f, 314.0f);

    /**
    * Set custom sprite sheet info. Use Godot sprite sheet tool to get these values.
    * 
    * Variable overview:
    * - Frame index: The first frame that should be displayed. 
    * - Frame width: Width of the sprite sheet frame
    * - Frame height: Height of the sprite sheet frame
    * - x_offset (y_offset): The location of the start of the sprite. Can be x or y depending on the sprite sheet.
    * 
    * TODO: Abstract these into a dedicated class. Part of the sprite node class?
    * 
    **/
    int sprite_0_frame_index = 0;
    int sprite_0_frame_width = 64;
    int sprite_0_frame_height = 64;
    int sprite_0_x_offset = sprite_0_frame_index * sprite_0_frame_width;

    // Set data within sprite node 
    // TODO: Wrap this function too
    sprite_0.get_child<0>().set_source_rect(sprite_0_x_offset, 
                                            0, 
                                            sprite_0_frame_width, 
                                            sprite_0_frame_height);

    /**
     * Sprite 1 frame info (note that this one uses a y-offset)
    **/
    int sprite_1_frame_index = 0;
    int sprite_1_frame_width = 64;
    int sprite_1_frame_height = 64;
    int sprite_1_y_offset = sprite_1_frame_index * sprite_1_frame_width;

    sprite_1.get_child<0>().set_source_rect(sprite_1_y_offset, 
                                            0, 
                                            sprite_1_frame_width, 
                                            sprite_1_frame_height);

    // Flip sprite 1 for testing
    sprite_1.get_child<0>().set_flip_horizontal(true);
    sprite_1.get_child<0>().set_flip_vertical(true);

    // Reset SDLInfo and texture node to nullptr within the scene state struct
    scene_state_.reset();

    // Set scene state SDLInfo data to new SDLInfo data
    scene_state_.sdl_info = sdl_info_;

    // Initialize root node with new scene state (texture node is still null)
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
