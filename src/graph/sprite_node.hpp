#ifndef GRAPH_SPRITE_NODE_HPP
#define GRAPH_SPRITE_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "graph/texture_node.hpp"
#include "graph/geometry_node.hpp"
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

    // Wrapped functions
    void set_filepath(const std::string &path);
    void set_blend(bool blend);
    void set_blend_alpha(uint8_t alpha);
    void set_color_mods(const uint8_t mods[3]);
    void set_top_left(float x, float y);
    void set_top_right(float x, float y);
    void set_bottom_left(float x, float y);

private:
    TextureNode *get_texture_node();
    GeometryNode *get_geometry_node();

};

template <typename... ChildrenTs>
using SpriteNodeT = NodeT<SpriteNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_SPRITE_NODE_HPP
