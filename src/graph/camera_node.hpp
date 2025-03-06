#ifndef GRAPH_CAMERA_NODE_HPP
#define GRAPH_CAMERA_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "graph/transform_node.hpp"
#include "platform/camera.hpp"

namespace cge
{

class CameraNode : public Node
{
  public:
    CameraNode() : camera_(0.0f, 0.0f, 10.0f, 10.0f) {}
    ~CameraNode() = default;

    // Overrides
    void init(SceneState &scene_state) override;
    void destroy() override;
    void draw(SceneState &scene_state) override;
    void update(SceneState &scene_state) override;

    // Accessing camera
    Camera &get_camera();

    // Camera following features
    void set_target(TransformNode *target, bool follow = true);
    void set_follow_smoothness(float smoothness);
    bool is_following_target() const;

private:
    Camera camera_;
    TransformNode *target_transform_{nullptr};
    bool           follow_target_{false};
    float          follow_smoothness_{0.1f};


};

template <typename... ChildrenTs>
using CameraNodeT = NodeT<CameraNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_CAMERA_NODE_HPP
