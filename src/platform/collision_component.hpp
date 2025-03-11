#ifndef PLATFORM_COLLISION_COMPONENT_HPP
#define PLATFORM_COLLISION_COMPONENT_HPP

#include "platform/math.hpp"

namespace cge
{

class TransformNode;

// Base class for collision components
class CollisionComponent
{
public:
    enum class Type
    {
        CIRCLE,
        AABB
    };

    CollisionComponent(TransformNode *owner);
    virtual ~CollisionComponent() = default;

    // Check collision with another component
    virtual bool collides_with(const CollisionComponent &other) const = 0;

    // Get the type of collision component
    virtual Type get_type() const = 0;

    // Get world position considering the owner's transform
    Vector2 get_world_position() const;

    // Get owner transform node
    TransformNode *get_owner() const { return owner_; }

    // Enable/disable collision detection for this component
    void set_enabled(bool enabled) { enabled_ = enabled; }
    bool is_enabled() const { return enabled_; }

    // Set/get local position offset from the transform's position
    void           set_offset(const Vector2 &offset) { offset_ = offset; }
    const Vector2 &get_offset() const { return offset_; }

protected:
    TransformNode *owner_;
    bool           enabled_{true};
    Vector2        offset_; // Offset from the transform's position
};

// Circle-based collision component
class CircleCollisionComponent : public CollisionComponent
{
public:
    CircleCollisionComponent(TransformNode *owner, float radius);

    bool collides_with(const CollisionComponent &other) const override;
    Type get_type() const override { return Type::CIRCLE; }

    // Get the circle in world space
    Circle get_world_circle() const;

    // Radius accessors
    float get_radius() const { return radius_; }
    void  set_radius(float radius) { radius_ = radius; }

private:
    float radius_;
};

// AABB-based collision component using min/max points
class AABBCollisionComponent : public CollisionComponent
{
public:
    AABBCollisionComponent(TransformNode *owner, const Vector2 &min, const Vector2 &max);

    bool collides_with(const CollisionComponent &other) const override;
    Type get_type() const override { return Type::AABB; }

    // Get the AABB in world space
    AABB2 get_world_aabb() const;

    // Min/Max accessors
    const Vector2 &get_min() const { return local_min_; }
    const Vector2 &get_max() const { return local_max_; }
    void           set_min(const Vector2 &min) { local_min_ = min; }
    void           set_max(const Vector2 &max) { local_max_ = max; }

private:
    Vector2 local_min_; // Min point relative to transform position
    Vector2 local_max_; // Max point relative to transform position
};

} // namespace cge

#endif // PLATFORM_COLLISION_COMPONENT_HPP
