#include "graph/transform_node.hpp"
#include "graph/sprite_node.hpp"
#include "platform/movement_controller.hpp"
#include "platform/collision_component.hpp"

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
    // Update movement controller if it exists
    if(movement_controller_) 
    { 
        movement_controller_->update(scene_state); 
        
        // Update associated sprite with movement state
        if (associated_sprite_)
        {
            associated_sprite_->set_movement_state(
                movement_controller_->is_moving(),
                movement_controller_->get_direction(),
                movement_controller_->is_facing_left()
            );
        }
    }

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

// Configure player controller. Use this TransformNode in the constructor.
void TransformNode::set_player_controlled()
{
    movement_controller_ = std::make_unique<PlayerController>(*this);
}

// Configure path controller. Use this TransformNode in the constructor.
void TransformNode::set_path_controlled(Path &path)
{
    auto path_controller = std::make_unique<PathController>(*this);
    path_controller->set_path(path);
    movement_controller_ = std::move(path_controller);
}

bool TransformNode::is_moving() const
{
    return movement_controller_ && movement_controller_->is_moving();
}

MoveDirection TransformNode::get_move_direction() const
{
    return movement_controller_ ? movement_controller_->get_direction() : MoveDirection::NONE;
}

bool TransformNode::is_facing_left() const
{
    return movement_controller_ ? movement_controller_->is_facing_left() : false;
}

void TransformNode::set_associated_sprite(SpriteNode *sprite) { associated_sprite_ = sprite; } 

// New collision component methods
CircleCollisionComponent *TransformNode::add_circle_collider(float radius)
{
    // Clean up existing collision component if it exists
    delete collision_component_;

    // Create a new circle collision component
    auto *collider = new CircleCollisionComponent(this, radius);
    collision_component_ = collider;
    return collider;
}

AABBCollisionComponent *TransformNode::add_aabb_collider(const Vector2 &min, const Vector2 &max)
{
    // Clean up existing collision component if it exists
    delete collision_component_;

    // Create a new AABB collision component
    auto *collider = new AABBCollisionComponent(this, min, max);
    collision_component_ = collider;
    return collider;
}

} // namespace cge
