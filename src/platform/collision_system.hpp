#ifndef PLATFORM_COLLISION_SYSTEM_HPP
#define PLATFORM_COLLISION_SYSTEM_HPP

#include "platform/collision_component.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace cge
{

// Forward declarations
class CollisionComponent;

// A pair of components that have collided
struct CollisionPair
{
    std::shared_ptr<CollisionComponent> first;
    std::shared_ptr<CollisionComponent> second;

    CollisionPair(std::shared_ptr<CollisionComponent> a, std::shared_ptr<CollisionComponent> b) 
        : first(a), second(b) {}
};

// Collision system to manage and check collisions between components
class CollisionSystem
{
  public:
    CollisionSystem() = default;
    ~CollisionSystem();

    // Add/remove collision components
    void add_component(std::shared_ptr<CollisionComponent> component);
    void remove_component(std::shared_ptr<CollisionComponent> component);

    // Check all registered components for collisions
    std::vector<CollisionPair> check_collisions();

    // Clear all components
    void clear();

  private:
    std::vector<std::shared_ptr<CollisionComponent>> components_;

};

} // namespace cge

#endif // PLATFORM_COLLISION_SYSTEM_HPP
