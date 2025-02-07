/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GRAPH_TEXTURE_NODE_HPP
#define GRAPH_TEXTURE_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"

#include "platform/sdl.h"

namespace cge
{
class TextureNode : public Node
{
  public:
    TextureNode();

    ~TextureNode() = default;

    void init(SceneState &scene_state) override;

    void destroy() override;

    void draw(SceneState &scene_state) override;
    
    void update(SceneState &scene_state) override;

    SDL_Texture *sdl_texture();

    int width() const;

    int height() const;

    void set_filepath(const std::string &filepath);

    void set_color_mods(const uint8_t mods[3]);
    void set_blend(bool blend);
    void set_blend_alpha(uint8_t alpha);

    // For working with sprite sheets
    void set_source_rect(int x, int y, int w, int h);
    bool use_source_rect() const;
    SDL_Rect* get_src_rect();

    // Flip textures
    void set_flip_horizontal(bool flip_h) { flip_horizontal_ = flip_h; }
    void set_flip_vertical(bool flip_v) { flip_vertical_ = flip_v; }
    bool flip_horizontal() const { return flip_horizontal_; }
    bool flip_vertical() const { return flip_vertical_; }

  protected:
    SDL_Texture *texture_;
    int          width_;
    int          height_;
    std::string  filepath_;

    bool    apply_color_mod_;
    uint8_t color_mods_[3];

    bool    apply_blend_;
    uint8_t blend_alpha_;

    // For working with sprite sheets
    bool use_src_rect_{false};
    SDL_Rect src_rect_;

    // For flipping textures
    bool flip_horizontal_{false};
    bool flip_vertical_{false};
};

template <typename... ChildrenTs>
using TextureNodeT = NodeT<TextureNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_TEXTURE_NODE_HPP
