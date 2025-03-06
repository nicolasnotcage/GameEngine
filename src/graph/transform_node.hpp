#ifndef GRAPH_TRANSFORM_NODE_HPP
#define GRAPH_TRANSFORM_NODE_HPP

#include "graph/geometry_node.hpp"
#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "graph/texture_node.hpp"
#include "platform/math.hpp"

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

    void set_identity();
    void left_scale(float x, float y);
    void right_scale(float x, float y);
    void left_rotate_degrees(float angle_deg);
    void right_rotate_degrees(float angle_deg);
    void left_rotate(float rad_deg);
    void right_rotate(float rad_deg);
    void left_translate(float x, float y);
    void right_translate(float x, float y);

    Matrix3 &get_transform() { return transform_; }

private:
    Matrix3 transform_;
};

template <typename... ChildrenTs>
using TransformNodeT = NodeT<TransformNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_TRANSFORM_NODE_HPP
