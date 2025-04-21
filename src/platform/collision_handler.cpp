/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/collision_handler.hpp"
#include "graph/transform_node.hpp"
#include <algorithm>

namespace cge
{

CollisionHandler::CollisionHandler(CollisionSystem& collision_system)
    : collision_system_(collision_system)
{
    // Set default handlers that do nothing
    boundary_handler_ = [](TransformNode*, TransformNode*) {};
    entity_handler_ = [](TransformNode*, TransformNode*) {};
    trigger_handler_ = [](TransformNode*, TransformNode*) {};
}

void CollisionHandler::process_collisions()
{
    // Get collision pairs from the system
    auto collision_pairs = collision_system_.check_collisions();

    // Handle each collision pair
    for(const auto& pair : collision_pairs)
    {
        // Get transform nodes
        TransformNode* transform_a = pair.first->get_owner();
        TransformNode* transform_b = pair.second->get_owner();

        // Handle boundary collisions
        if(is_boundary_node(transform_a))
        {
            boundary_handler_(transform_b, transform_a);
        }
        else if(is_boundary_node(transform_b))
        {
            boundary_handler_(transform_a, transform_b);
        }
        // Handle trigger collisions
        else if(is_trigger_node(transform_a))
        {
            trigger_handler_(transform_b, transform_a);
        }
        else if(is_trigger_node(transform_b))
        {
            trigger_handler_(transform_a, transform_b);
        }
        // Handle entity-entity collisions
        else
        {
            entity_handler_(transform_a, transform_b);
        }
    }
}

void CollisionHandler::register_boundary_response(std::function<void(TransformNode*, TransformNode*)> handler)
{
    boundary_handler_ = handler;
}

void CollisionHandler::register_entity_response(std::function<void(TransformNode*, TransformNode*)> handler)
{
    entity_handler_ = handler;
}

void CollisionHandler::register_trigger_response(std::function<void(TransformNode*, TransformNode*)> handler)
{
    trigger_handler_ = handler;
}

void CollisionHandler::add_boundary_node(TransformNode* node)
{
    if(node && std::find(boundary_nodes_.begin(), boundary_nodes_.end(), node) == boundary_nodes_.end())
    {
        boundary_nodes_.push_back(node);
    }
}

void CollisionHandler::add_trigger_node(TransformNode* node)
{
    if(node && std::find(trigger_nodes_.begin(), trigger_nodes_.end(), node) == trigger_nodes_.end())
    {
        trigger_nodes_.push_back(node);
    }
}

bool CollisionHandler::is_boundary_node(TransformNode* node) const
{
    return std::find(boundary_nodes_.begin(), boundary_nodes_.end(), node) != boundary_nodes_.end();
}

bool CollisionHandler::is_trigger_node(TransformNode* node) const
{
    return std::find(trigger_nodes_.begin(), trigger_nodes_.end(), node) != trigger_nodes_.end();
}

} // namespace cge
