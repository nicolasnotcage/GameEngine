#include "graph/transform_node.hpp"
#include "platform/math.hpp"

namespace cge
{

void TransformNode::init(SceneState &scene_state) { init_children(scene_state); }
void TransformNode::draw(SceneState &scene_state) 
{ 
    scene_state.matrix_stack.push();
    scene_state.matrix_stack.top() *= transform_;
    draw_children(scene_state); 
    scene_state.matrix_stack.pop();
}

void TransformNode::destroy()
{
    destroy_children();
    clear_children();
}

void TransformNode::update(SceneState &scene_state) 
{ 
    update_children(scene_state); 
}

// TODO: Test this
void TransformNode::set_identity() { transform_.set_identity(); }

void TransformNode::left_scale(float x, float y) { transform_.left_scale(x, y); }

void TransformNode::right_scale(float x, float y) { transform_.right_scale(x, y); }

// Convert degrees to radians and perform rotation
void TransformNode::left_rotate_degrees(float angle_deg)
{
    float rad_deg = cge::degrees_to_radians(angle_deg);
    transform_.left_rotate(rad_deg);
}

void TransformNode::right_rotate_degrees(float angle_deg)
{
    float rad_deg = cge::degrees_to_radians(angle_deg);
    transform_.right_rotate(rad_deg);
}

void TransformNode::left_rotate(float rad_deg) { transform_.left_rotate(rad_deg); }
void TransformNode::right_rotate(float rad_deg) { transform_.right_rotate(rad_deg); }

void TransformNode::left_translate(float x, float y) { transform_.left_translate(x, y); }
void TransformNode::right_translate(float x, float y) { transform_.right_translate(x, y); }
} // namespace cge
