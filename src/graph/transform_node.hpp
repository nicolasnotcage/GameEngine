#ifndef GRAPH_SPRITE_NODE_HPP
#define GRAPH_SPRITE_NODE_HPP

#include "graph/geometry_node.hpp"
#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "graph/texture_node.hpp"

namespace cge
{

class TransformNode : public Node
{
  public:
    TransformNode() = default;
    ~TransformNode() = default;

    // Overrides
    void init(SceneState &scene_state) override;
    void destroy() override;
    void draw(SceneState &scene_state) override;
    void update(SceneState &scene_state) override;
};

template <typename... ChildrenTs>
using TransformNodeT = NodeT<TransformNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_SPRITE_NODE_HPP
