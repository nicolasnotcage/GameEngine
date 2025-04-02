#include "graph/transform_node.hpp"
#include "graph/sprite_node.hpp"
#include "platform/movement_controller.hpp"
#include "platform/collision_component.hpp"
#include "platform/audio_component.hpp"
#include "system/serializer.hpp"

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

    // TODO: Use smart pointer here to prevent the need for this. 
    if(audio_component_)
    {
        delete audio_component_;
        audio_component_ = nullptr;
    }
}

void TransformNode::update(SceneState &scene_state) 
{ 
    // Update movement controller if it exists
    if(movement_controller_) 
    { 
        // Store previous transform before processing movement
        store_previous_transform();

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

// Configure player controller
void TransformNode::set_player_controlled()
{
    movement_controller_ = std::make_unique<PlayerController>(*this);
}

// Configure path controller
void TransformNode::set_path_controlled(Path &path)
{
    auto path_controller = std::make_unique<PathController>(*this);
    path_controller->set_path(path);
    movement_controller_ = std::move(path_controller);
}

// Returns if the stored movement controller is currently moving.
bool TransformNode::is_moving() const
{
    return movement_controller_ && movement_controller_->is_moving();
}

// Returns the movement direction of the stored movement controller. 
MoveDirection TransformNode::get_move_direction() const
{
    return movement_controller_ ? movement_controller_->get_direction() : MoveDirection::NONE;
}

// Returns if the stored movement controller is currently facing left. 
bool TransformNode::is_facing_left() const
{
    return movement_controller_ ? movement_controller_->is_facing_left() : false;
}

// Associates this TransformNode with a SpriteNode. 
void TransformNode::set_associated_sprite(SpriteNode *sprite) { associated_sprite_ = sprite; } 

//------------------------------
// Collision component methods
//------------------------------

// Create and return a pointer to a CircleCollisionComponent. 
CircleCollisionComponent *TransformNode::add_circle_collider(float radius)
{
    auto                      collider = std::make_unique<CircleCollisionComponent>(this, radius);
    CircleCollisionComponent *raw_ptr = collider.get();
    collision_component_ = std::move(collider);
    return raw_ptr;
}

// Create and return a pointer to an AABBCollisionComponent. 
AABBCollisionComponent *TransformNode::add_aabb_collider(const Vector2 &min, const Vector2 &max)
{
    auto                    collider = std::make_unique<AABBCollisionComponent>(this, min, max);
    AABBCollisionComponent *raw_ptr = collider.get();
    collision_component_ = std::move(collider);
    return raw_ptr;
}

//------------------------------
// Audio component methods
//------------------------------
AudioComponent *TransformNode::add_audio_component() 
{ 
    audio_component_ = new AudioComponent(this);
    return audio_component_;
}

//------------------------------
// Serialization Implementation
//------------------------------
void TransformNode::serialize(Serializer& serializer) const
{
    // Serialize position
    serializer.write("position_x", get_position_x());
    serializer.write("position_y", get_position_y());
}

void TransformNode::deserialize(Serializer& serializer)
{
    // Deserialize position
    float x = 0.0f, y = 0.0f;
    if (serializer.read("position_x", x) && serializer.read("position_y", y)) 
    {
        std::cout << "TransformNode::deserialize - Setting position to: " << x << ", " << y << std::endl;
        set_position(x, y);
        
        // Double-check that the position was set correctly
        std::cout << "TransformNode::deserialize - Position after set: " 
                  << get_position_x() << ", " << get_position_y() << std::endl;
    }
    else
    {
        std::cout << "TransformNode::deserialize - Failed to read position from serializer" << std::endl;
    }
}

} // namespace cge
