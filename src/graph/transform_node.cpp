#include "graph/transform_node.hpp"

namespace cge
{

void TransformNode::init(SceneState &scene_state) { init_children(scene_state); }
void TransformNode::draw(SceneState &scene_state) { draw_children(scene_state); }

void TransformNode::destroy()
{
    destroy_children();
    clear_children();
}

void TransformNode::update(SceneState &scene_state) 
{ 
    update_children(scene_state); 
}

} // namespace cge
