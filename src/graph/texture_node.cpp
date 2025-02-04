/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "graph/texture_node.hpp"

#include "image/image.hpp"
#include "system/file_locator.hpp"

namespace cge
{

TextureNode::TextureNode() :
    texture_{nullptr},
    width_{0},
    height_{0},
    filepath_{""},
    apply_color_mod_{false},
    color_mods_{0},
    apply_blend_{false},
    blend_alpha_{0}
{
}

void TextureNode::init(SceneState &scene_state)
{
    auto file_info = locate_path_for_filename(filepath_);
    auto result = image::create_texture(*scene_state.sdl_info, file_info.path);

    texture_ = result.texture;
    width_ = result.width;
    height_ = result.height;

    init_children(scene_state);
}

void TextureNode::destroy()
{
    destroy_children();
    clear_children();

    SDL_DestroyTexture(texture_);
}

void TextureNode::draw(SceneState &scene_state)
{
    TextureNode *prex_texture_node = scene_state.texture_node;
    scene_state.texture_node = this;

    if(apply_blend_)
    {
        SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(texture_, blend_alpha_);
    }
    else { SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_NONE); }

    if(apply_color_mod_)
    {
        SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_BLEND);
        SDL_SetTextureColorMod(texture_, color_mods_[0], color_mods_[1], color_mods_[2]);
    } 

    draw_children(scene_state);

    scene_state.texture_node = prex_texture_node;
}

SDL_Texture *TextureNode::sdl_texture() { return texture_; }

int TextureNode::width() const { return width_; }

int TextureNode::height() const { return height_; }

void TextureNode::set_filepath(const std::string &filepath) { filepath_ = filepath; }

void TextureNode::set_color_mods(const uint8_t mods[3])
{
    apply_color_mod_ = true;
    color_mods_[0] = mods[0];
    color_mods_[1] = mods[1];
    color_mods_[2] = mods[2];
}

void TextureNode::set_blend(bool blend) { apply_blend_ = blend; }

void TextureNode::set_blend_alpha(uint8_t alpha) { blend_alpha_ = alpha; }

// Define a sub-section of the texture to be displayed
// TODO: Should this be handled by sprite nodes? Also need to store a frame index.
void TextureNode::set_source_rect(int x, int y, int w, int h) 
{ 
    use_src_rect_ = true; 
    src_rect_.x = x;
    src_rect_.y = y;
    src_rect_.w = w;
    src_rect_.h = h;
}

// Used by geometry nodes when determining what to draw
bool TextureNode::use_source_rect() const
{ 
    return use_src_rect_; 
}

// Returns pointer to source rectangle
SDL_Rect* TextureNode::get_src_rect() { return &src_rect_; }

} // namespace cge
