#ifndef GRAPH_SPRITE_NODE_HPP
#define GRAPH_SPRITE_NODE_HPP

#include "graph/geometry_node.hpp"
#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "graph/texture_node.hpp"
namespace cge
{

/**
 * Generic sprite node class. Provides the basic functionality expected of an engine node
 * and allows clients to flip sprites vertically and horizontally.
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
};

/**
 * Templated sprite node class that provides wrapper functions for texture and geometry nodes.
 *
 * Assumes:
 *    1) A sprite node's first child is a texture node
 *    2) A texture node's first child is a geometry node
 *
 * Is there some way to make this more dynamic? Or is this a consequence of the static approach?
 *
 * TODO: Add sprite sheet animation functionality.
 *
 **/
template <typename... ChildrenTs>
class SpriteNodeT : public NodeT<SpriteNode, ChildrenTs...>
{
  public:
    // Wrapped texture node functions
    void set_filepath(const std::string &path)
    {
        auto &tex_node = this->template get_child<0>();
        tex_node.set_filepath(path);
    }

    void set_blend(bool blend)
    {
        auto &tex_node = this->template get_child<0>();
        tex_node.set_blend(blend);
    }

    void set_blend_alpha(uint8_t alpha)
    {
        auto &tex_node = this->template get_child<0>();
        tex_node.set_blend_alpha(alpha);
    }

    void set_color_mods(const uint8_t mods[3])
    {
        auto &tex_node = this->template get_child<0>();
        tex_node.set_color_mods(mods);
    }

    // Wrapped geometry node functions
    void set_top_left(float x, float y)
    {
        auto &tex_node = this->template get_child<0>();
        auto &geo_node = tex_node.template get_child<0>();
        geo_node.set_top_left(x, y);
    }

    void set_top_right(float x, float y)
    {
        auto &tex_node = this->template get_child<0>();
        auto &geo_node = tex_node.template get_child<0>();
        geo_node.set_top_right(x, y);
    }

    void set_bottom_left(float x, float y)
    {
        auto &tex_node = this->template get_child<0>();
        auto &geo_node = tex_node.template get_child<0>();
        geo_node.set_bottom_left(x, y);
    }
};

// Define sprite types
using BasicSprite = SpriteNodeT<TextureNodeT<GeometryNodeT<>>>;

} // namespace cge

#endif // GRAPH_SPRITE_NODE_HPP
