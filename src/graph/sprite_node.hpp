#ifndef GRAPH_SPRITE_NODE_HPP
#define GRAPH_SPRITE_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "graph/texture_node.hpp"
#include "graph/geometry_node.hpp"
namespace cge
{
class SpriteNode : public Node
{
public:
    SpriteNode() = default;
    ~SpriteNode() = default;

    // Overrides
    void init(SceneState &scene_state) override;
    void destroy() override;
    void draw(SceneState &scene_state) override;

    // Helper methods
    void set_filepath(const std::string &path);
    void set_transform(float x, float y, float angle);

    // Texture wrapper methods
    void set_color_mods(const uint8_t mods[3]);
    void set_blend(bool blend);
    void set_blend_alpha(uint8_t alpha);

    // Geometry wrapper methods
    void set_top_left(float x, float y);
    void set_top_right(float x, float y);
    void set_bottom_left(float x, float y); 
    
private:
    TextureNodeT<GeometryNodeT<>> texture_node;
    float x{0.0f};
    float y{0.0f};
    float angle{0.0f};
};

// Templated sprite node
template <typename... ChildrenTs>
using SpriteNodeT = NodeT<SpriteNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_SPRITE_NODE_HPP
