#ifndef PLATFORM_COLLISION_SYSTEM_HPP
#define PLATFORM_COLLISION_SYSTEM_HPP

#include "platform/collision_component.hpp"

#include <functional>
#include <vector>

namespace cge
{

// Forward declarations
class CollisionComponent;

// A pair of components that have collided
struct CollisionPair
{
    CollisionComponent *first;
    CollisionComponent *second;

    CollisionPair(CollisionComponent *a, CollisionComponent *b) : first(a), second(b) {}
};

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
    std::vector<CollisionPair> check_collisions();

    // Clear all components
    void clear();

  private:
    std::vector<CollisionComponent *> components_;

};

} // namespace cge

#endif // PLATFORM_COLLISION_SYSTEM_HPP
