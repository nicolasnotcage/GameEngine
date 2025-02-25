#include "graph/camera_node.hpp"
#include "platform/math.hpp"

namespace cge
{

void CameraNode::init(SceneState &scene_state) 
{ 
    // Set this camera as the active camera within the scene state
    scene_state.active_camera = this;

    // Init children
    init_children(scene_state); 
}

void CameraNode::destroy()
{
    destroy_children();
    clear_children();
}

void CameraNode::draw(SceneState &scene_state)
{
    // Store previous active camera and set this camera as the active camera
    CameraNode *old_camera = scene_state.active_camera;
    scene_state.active_camera = this;

    // Update matrix stack and draw children
    scene_state.matrix_stack.push();
    draw_children(scene_state);

    // Pop matrix stack and restore previous camera
    scene_state.matrix_stack.pop();
    scene_state.active_camera = old_camera;
}

void CameraNode::update(SceneState &scene_state) 
{
    // TODO: Add input-based camera movement

    // Update children
    update_children(scene_state); 
}

Camera &CameraNode::get_camera() { return camera_; }

} // namespace cge
