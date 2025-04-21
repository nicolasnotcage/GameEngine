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
class TransformNode;

// Collision system to manage and check collisions between components
class CollisionSystem
{
  public:
    // Collision type enum
    enum class CollisionType
    {
        ENTITY,
        BOUNDARY,
        TRIGGER
    };

    CollisionSystem();
    ~CollisionSystem();

    // Add/remove collision components with type
    void add_component(std::shared_ptr<CollisionComponent> component, 
                       CollisionType type = CollisionType::ENTITY);
    void remove_component(std::shared_ptr<CollisionComponent> component);

    // Register response handlers for different collision types
    void register_boundary_response(std::function<void(TransformNode*, TransformNode*)> handler);
    void register_entity_response(std::function<void(TransformNode*, TransformNode*)> handler);
    void register_trigger_response(std::function<void(TransformNode*, TransformNode*)> handler);

    // Process all collisions (detection and response)
    void process_collisions();

    // Clear all components
    void clear();

    // Get collision type for a component
    CollisionType get_component_type(const CollisionComponent* component) const;

  private:
    // Component storage with type information
    struct ComponentEntry
    {
        std::shared_ptr<CollisionComponent> component;
        CollisionType type;
    };
    
    std::vector<ComponentEntry> components_;
    
    // Response handlers
    std::function<void(TransformNode*, TransformNode*)> boundary_handler_;
    std::function<void(TransformNode*, TransformNode*)> entity_handler_;
    std::function<void(TransformNode*, TransformNode*)> trigger_handler_;
};

} // namespace cge

#endif // PLATFORM_COLLISION_SYSTEM_HPP
