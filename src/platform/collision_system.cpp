/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/collision_system.hpp"
#include <algorithm>

namespace cge
{

CollisionSystem::~CollisionSystem() { clear(); }

void CollisionSystem::add_component(CollisionComponent *component)
{
    if(component)
    {
        // Check if component is already in the list
        auto it = std::find(components_.begin(), components_.end(), component);
        if(it == components_.end()) { components_.push_back(component); }
    }
}

void CollisionSystem::remove_component(CollisionComponent *component)
{
    auto it = std::find(components_.begin(), components_.end(), component);
    if(it != components_.end()) { components_.erase(it); }
}

std::vector<CollisionPair> CollisionSystem::check_collisions()
{
    std::vector<CollisionPair> colliding_pairs;

    // Check all components against each other
    // TODO: O(n^2). Optimize?
    for(size_t i = 0; i < components_.size(); ++i)
    {
        for(size_t j = i + 1; j < components_.size(); ++j)
        {
            CollisionComponent *a = components_[i];
            CollisionComponent *b = components_[j];

            // Skip if either component is disabled
            if(!a->is_enabled() || !b->is_enabled()) { continue; }

            // Check for collision
            if(a->collides_with(*b))
            {
                // Add the pair to our result list
                colliding_pairs.emplace_back(a, b);
            }
        }
    }

    return colliding_pairs;
}

void CollisionSystem::clear()
{
    components_.clear();
}

} // namespace cge
