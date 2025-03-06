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
#include "platform/animation.hpp"

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

    // Sprite sheet functionality
    void define_frame(uint32_t frame_id, int x, int y, int width, int height);
    void define_grid(int cols, int rows, int frame_width, int frame_height);
    void set_current_frame(uint32_t frame_id);
    const std::unordered_map<uint32_t, Frame> &get_frames() const;
    uint32_t get_current_frame_id() const;

  protected:
    SDL_Texture *texture_;
    int          width_;
    int          height_;
    std::string  filepath_;

    bool    apply_color_mod_;
    uint8_t color_mods_[3];

    bool    apply_blend_;
    uint8_t blend_alpha_;

    // Sprite sheet members
    std::unordered_map<uint32_t, Frame> frames_;
    uint32_t current_frame_id_;
    bool is_sprite_sheet_;
};

template <typename... ChildrenTs>
using TextureNodeT = NodeT<TextureNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_TEXTURE_NODE_HPP
