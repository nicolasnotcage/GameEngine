/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/collision_component.hpp"
#include "graph/transform_node.hpp"

namespace cge
{

CollisionComponent::CollisionComponent(TransformNode *owner) : owner_(owner), offset_(0.0f, 0.0f) {}

Vector2 CollisionComponent::get_world_position() const
{
    if(!owner_) { return offset_; }

    // Get the transform matrix from the owner
    Matrix3 transform = owner_->get_transform();

    // Extract the translation component (position)
    Vector2 position(transform.a[6], transform.a[7]);

    // Add the local offset
    return position + offset_;
}

// CircleCollisionComponent implementation
CircleCollisionComponent::CircleCollisionComponent(TransformNode *owner, float radius) :
    CollisionComponent(owner), radius_(radius)
{
}

bool CircleCollisionComponent::collides_with(const CollisionComponent &other) const
{
    if(!enabled_ || !other.is_enabled()) { return false; }

    if(other.get_type() == Type::CIRCLE)
    {
        const auto &circle_other = static_cast<const CircleCollisionComponent &>(other);
        return get_world_circle().intersects(circle_other.get_world_circle());
    }
    
    // TODO: Could extend to accommodate other collision component types
    return false;
}

Circle CircleCollisionComponent::get_world_circle() const
{
    Vector2 world_pos = get_world_position();
    return Circle(world_pos, radius_);
}

// AABBCollisionComponent implementation
AABBCollisionComponent::AABBCollisionComponent(TransformNode *owner,
                                               const Vector2 &min,
                                               const Vector2 &max) :
    CollisionComponent(owner), local_min_(min), local_max_(max)
{
}

bool AABBCollisionComponent::collides_with(const CollisionComponent &other) const
{
    if(!enabled_ || !other.is_enabled()) { return false; }

    if(other.get_type() == Type::AABB)
    {
        const auto &aabb_other = static_cast<const AABBCollisionComponent &>(other);
        return get_world_aabb().intersects(aabb_other.get_world_aabb());
    }

    // TODO: Could extend to accommodate other collision component types
    return false;
}

AABB2 AABBCollisionComponent::get_world_aabb() const
{
    Vector2 world_pos = get_world_position();
    return AABB2(Vector2(world_pos.x + local_min_.x, world_pos.y + local_min_.y),
                 Vector2(world_pos.x + local_max_.x, world_pos.y + local_max_.y));
}

} // namespace cge
