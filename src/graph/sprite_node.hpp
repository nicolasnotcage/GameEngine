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
 **/
template <typename... ChildrenTs>
class SpriteNodeT : public NodeT<SpriteNode, ChildrenTs...>
{
  public:
    // Wrapped texture node functions
    void set_filepath(const std::string &path)
    {
        if (auto* tex_node = this->get_texture_node()) tex_node->set_filepath(path);
    }

    void set_blend(bool blend)
    {
        if(auto *tex_node = this->get_texture_node()) tex_node->set_blend(blend);
    }

    void set_blend_alpha(uint8_t alpha)
    {
        if(auto *tex_node = this->get_texture_node()) tex_node->set_blend_alpha(alpha);
    }

    void set_color_mods(const uint8_t mods[3])
    {
        if(auto *tex_node = this->get_texture_node()) tex_node->set_color_mods(mods);
    }

    // Wrapped geometry node functions
    void set_top_left(float x, float y)
    {
        if(auto *geo_node = this->get_geometry_node()) geo_node->set_top_left(x, y);
    }

    void set_top_right(float x, float y)
    {
        if(auto *geo_node = this->get_geometry_node()) geo_node->set_top_right(x, y);
    }

    void set_bottom_left(float x, float y)
    {
        if(auto *geo_node = this->get_geometry_node()) geo_node->set_bottom_left(x, y);
    }

private:
    // Utility functions to get texture and geometry node children
    TextureNode* get_texture_node()
    {
        if (auto* tex_node = this->template get_child_by_type<TextureNode>())
        { 
            return tex_node;
        }
        
        std::cerr << "Warning: SpriteNode has no instance of TextureNode.\n";
        return nullptr;

    }

    GeometryNode *get_geometry_node() 
    { 
        if(auto *tex_node = this->get_texture_node())
        {
            if (auto* geo_node = tex_node->template get_child_by_type<GeometryNode>())
            {
                return geo_node;
            }   
        }

        std::cerr << "Warning: TextureNode has no instance of GeometryNode.\n";
        return nullptr;
    }
};

// Define sprite types
using BasicSprite = SpriteNodeT<TextureNodeT<GeometryNodeT<>>>;

} // namespace cge

#endif // GRAPH_SPRITE_NODE_HPP
