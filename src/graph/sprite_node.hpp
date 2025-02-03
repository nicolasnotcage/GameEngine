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

private:
    std::shared_ptr<TextureNode> texture_node;
    std::shared_ptr<GeometryNode> geometry_node;

    float x;
    float y;
    float angle;
};

// Templated sprite node
template <typename... ChildrenTs>
using SpriteNodeT = NodeT<SpriteNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_SPRITE_NODE_HPP
