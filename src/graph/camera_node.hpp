#ifndef GRAPH_CAMERA_NODE_HPP
#define GRAPH_CAMERA_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"
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

private:
    Camera camera_;
};

template <typename... ChildrenTs>
using CameraNodeT = NodeT<CameraNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_CAMERA_NODE_HPP
