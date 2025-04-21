#ifndef PLATFORM_COLLISION_HANDLER_HPP
#define PLATFORM_COLLISION_HANDLER_HPP

#include "platform/collision_system.hpp"
#include <functional>

namespace cge
{

// Forward declarations
class TransformNode;

// Simple collision handler to abstract collision response logic
class CollisionHandler
{
public:
    CollisionHandler(CollisionSystem& collision_system);
    ~CollisionHandler() = default;

    // Process all collisions
    void process_collisions();

    // Register response handlers for different collision types
    void register_boundary_response(std::function<void(TransformNode*, TransformNode*)> handler);
    void register_entity_response(std::function<void(TransformNode*, TransformNode*)> handler);
    void register_trigger_response(std::function<void(TransformNode*, TransformNode*)> handler);

    // Set specific transform nodes to be recognized as boundaries or triggers
    void add_boundary_node(TransformNode* node);
    void add_trigger_node(TransformNode* node);

    // Check if a node is a boundary or trigger
    bool is_boundary_node(TransformNode* node) const;
    bool is_trigger_node(TransformNode* node) const;

private:
    CollisionSystem& collision_system_;
    
    // Response handlers
    std::function<void(TransformNode*, TransformNode*)> boundary_handler_;
    std::function<void(TransformNode*, TransformNode*)> entity_handler_;
    std::function<void(TransformNode*, TransformNode*)> trigger_handler_;

    // Collections of special nodes
    std::vector<TransformNode*> boundary_nodes_;
    std::vector<TransformNode*> trigger_nodes_;
};

} // namespace cge

#endif // PLATFORM_COLLISION_HANDLER_HPP
