#ifndef PLATFORM_COLLISION_SYSTEM_HPP
#define PLATFORM_COLLISION_SYSTEM_HPP

#include "platform/collision_component.hpp"

#include <functional>
#include <vector>

namespace cge
{

// Forward declarations
class CollisionComponent;

// Collision system to manage and check collisions between components
class CollisionSystem
{
  public:
    CollisionSystem() = default;
    ~CollisionSystem();

    // Add/remove collision components
    void add_component(CollisionComponent *component);
    void remove_component(CollisionComponent *component);

    // Check all registered components for collisions
    void check_collisions();

    // Callback type for collision responses
    using CollisionCallback = std::function<void(CollisionComponent *, CollisionComponent *)>;

    // Register a callback for collision responses
    void register_collision_callback(CollisionCallback callback);

    // Clear all components and callbacks
    void clear();

  private:
    std::vector<CollisionComponent *> components_;
    std::vector<CollisionCallback>    callbacks_;
};

} // namespace cge

#endif // PLATFORM_COLLISION_SYSTEM_HPP
