#ifndef GRAPH_SPRITE_NODE_HPP
#define GRAPH_SPRITE_NODE_HPP

#include "graph/geometry_node.hpp"
#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "graph/texture_node.hpp"
#include <iostream>

namespace cge
{

/**
 * SpriteNode orchestrates the relationship between a texture and its geometry. It provides a unified
 * interface for manipulating a 2D visual game entity. 
 **/
class SpriteNode : public Node
{
public:
SpriteNode() = default;
~SpriteNode() = default;

// Overrides
void init(SceneState &scene_state) override;
void destroy() override;
void draw(SceneState &scene_state) override;
void update(SceneState &scene_state) override;

// Texture manipulation
void set_filepath(const std::string &path);
void set_blend(bool blend);
void set_blend_alpha(uint8_t alpha);
void set_color_mods(const uint8_t mods[3]);

// Geometry manipulation
void set_top_left(float x, float y);
void set_top_right(float x, float y);
void set_bottom_left(float x, float y);

// Configure texture flipping
void set_flip_horizontal(bool flip_h);
void set_flip_vertical(bool flip_v);

// Configure sprite sheet dimensions
void set_source_rect(int x, int y, int w, int h);

protected:
// Protected accessor methods for derived classes
TextureNode  *get_texture_node();
GeometryNode *get_geometry_node();
};

/**
 * Factory function to create a standard sprite setup with a texture and geometry node.
 **/
inline std::shared_ptr<SpriteNode> create_basic_sprite()
{
    auto sprite = std::make_shared<SpriteNode>();
    auto texture = std::make_shared<TextureNode>();
    auto geometry = std::make_shared<GeometryNode>();

    texture->add_child(geometry);
    sprite->add_child(texture);

    return sprite;
}

template <typename... ChildrenTs>
using SpriteNodeT = NodeT<SpriteNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_SPRITE_NODE_HPP
