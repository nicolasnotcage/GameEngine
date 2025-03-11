/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/static_scene.hpp"
#include "platform/animation.hpp"
#include "platform/collision_component.hpp" 
#include "platform/collision_system.hpp"

#include <iostream>

namespace cge
{

void StaticScene::init(SDLInfo *sdl_info, IoHandler *io_handler)
{
    sdl_info_ = sdl_info;
    io_handler_ = io_handler;

    SDL_SetRenderDrawColor(sdl_info->renderer, 28, 40, 51, 0);
    SDL_SetRenderDrawBlendMode(sdl_info->renderer, SDL_BLENDMODE_BLEND);

    // Reset scene state
    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;
    scene_state_.io_handler = io_handler_;

    // Initialize all sprite textures

    // Golem walk texture
    golem_walk_texture_.set_filepath("images/golem_walk.png");
    golem_walk_texture_.set_blend(true);
    golem_walk_texture_.set_blend_alpha(200);
    golem_walk_texture_.define_grid(7, 1, 64, 64);
    golem_walk_texture_.init(scene_state_);

    // Golem idle texture
    golem_idle_texture_.set_filepath("images/golem_idle.png");
    golem_idle_texture_.set_blend(true);
    golem_idle_texture_.set_blend_alpha(200);
    golem_idle_texture_.define_grid(12, 1, 64, 64);
    golem_idle_texture_.init(scene_state_);

    // Witch run texture
    witch_run_texture_.set_filepath("images/witch_run.png");
    witch_run_texture_.set_blend(true);
    witch_run_texture_.set_blend_alpha(200);
    witch_run_texture_.define_grid(1, 6, 64, 64);
    witch_run_texture_.init(scene_state_);

    // Camera setup
    auto &camera = root_.get_child<0>();
    camera.get_camera().set_dimensions(20.0f, 15.0f);
    camera.get_camera().set_position(0.0f, 0.0f);

    // Set camera to print world coordinates on click; used for testing and defining collision boundaries
    camera.set_print_on_click(true);

    // Configure game map
    auto &game_map_transform = camera.get_child<0>();
    auto &game_map_tex = game_map_transform.get_child<0>();

    // Set texture filepath and scale it large
    game_map_tex.set_filepath("images/game_map.png");
    game_map_transform.right_scale(71.0f, 40.0f);

    // Get transforms
    auto &golem_transform = camera.get_child<1>();
    auto &witch_transform = camera.get_child<2>();

    // Position golem and witch
    golem_transform.right_translate(1.0f, 0.0f);
    golem_transform.right_scale(3.0f, 3.0f);

    witch_transform.right_translate(3.0f, 0.0f);
    witch_transform.right_scale(3.0f, 3.0f);

    // Setup animations for both characters
    setup_golem_animations();
    setup_witch_animations();

    // Setup collision components and responses
    setup_collisions();
    setup_trigger_zones();

    // Set camera to follow golem
    camera.set_target(&golem_transform, true);
    camera.set_follow_smoothness(1.0f);

    // Initialize root node
    root_.init(scene_state_);
}

void StaticScene::setup_golem_animations()
{
    auto &camera = root_.get_child<0>();
    auto &golem_transform = camera.get_child<1>();
    auto &golem_sprite = golem_transform.get_child<0>();

    // Create walk animation
    Animation walk_animation("walk", true);
    for(int i = 0; i < 7; i++) { walk_animation.add_frame(i, 10); }

    // Create idle animation
    Animation idle_animation("idle", true);
    for(int i = 0; i < 12; i++) { idle_animation.add_frame(i, 10); }

    // Add animations with their respective textures
    golem_sprite.add_animation_with_texture(walk_animation, &golem_walk_texture_);
    golem_sprite.add_animation_with_texture(idle_animation, &golem_idle_texture_);

    // Set initial texture
    golem_sprite.set_texture(&golem_idle_texture_);

    // Start playing idle animation by default
    golem_sprite.play("idle");

    // Set golem as player-controlled
    golem_transform.set_player_controlled();
    golem_transform.set_associated_sprite(&golem_sprite);
}

void StaticScene::setup_witch_animations()
{
    auto &camera = root_.get_child<0>();
    auto &witch_transform = camera.get_child<2>();
    auto &witch_sprite = witch_transform.get_child<0>();

    // Create run animation
    Animation walk_animation("walk", true);
    for(int i = 0; i < 6; i++) { walk_animation.add_frame(i, 10); }

    // Add animations with their respective textures
    witch_sprite.add_animation_with_texture(walk_animation, &witch_run_texture_);

    // Set initial texture
    witch_sprite.set_texture(&witch_run_texture_);

    // Start playing run animation by default
    witch_sprite.play("run");

    // Setup path for automated movement
    witch_path_.add_point(3.0f, 0.0f, 1.0f); // Start here
    witch_path_.add_point(8.0f, 0.0f, 0.5f); // Move right
    witch_path_.add_point(8.0f, 2.0f, 0.5f); // Move down
    witch_path_.add_point(3.0f, 2.0f, 0.5f); // Move left
    witch_path_.add_point(3.0f, 0.0f, 1.0f); // Back to start
    witch_path_.set_looping(true);

    // Set witch as path controlled and connect sprite
    witch_transform.set_path_controlled(witch_path_);
    witch_transform.set_associated_sprite(&witch_sprite);
}

void StaticScene::setup_collisions()
{
    auto &camera = root_.get_child<0>();
    auto &game_map_transform = camera.get_child<0>();
    auto &golem_transform = camera.get_child<1>();
    auto &witch_transform = camera.get_child<2>();

    // Configure world boundaries
    // 1. Bottom boundary (water on the bottom of the map)
    AABBCollisionComponent *boundary =
        game_map_transform.add_aabb_collider(Vector2(-35.5, 18.5), Vector2(35.4, 20));

    // TODO: Better map boundary handling
    // 2. Left boundary
    // 3. Right boundary
    // 4. Top boundary

    // Add collision component to golem (player)
    AABBCollisionComponent *golem_collider =
        golem_transform.add_aabb_collider(Vector2(-0.5f, -1.0f), Vector2(0.5f, 1.0f));

    // Add collision component to witch (NPC)
    CircleCollisionComponent *witch_collider = witch_transform.add_circle_collider(1.0f);

    // Register components with the collision system
    collision_system_.add_component(boundary);
    collision_system_.add_component(golem_collider);
    collision_system_.add_component(witch_collider);
}

void StaticScene::setup_trigger_zones()
{
    auto &camera = root_.get_child<0>();
    auto &zone_transform = camera.get_child<3>();

    // Position zone along witch's path
    zone_transform.right_translate(6.0f, 1.0f);

    // Add circular collider to zone
    CircleCollisionComponent *zone_collider = zone_transform.add_circle_collider(0.75f);
    collision_system_.add_component(zone_collider);
}

void StaticScene::destroy()
{
    root_.destroy();
    golem_walk_texture_.destroy();
    golem_idle_texture_.destroy();
    witch_run_texture_.destroy();
}

void StaticScene::render()
{
    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;
    scene_state_.io_handler = io_handler_;

    root_.draw(scene_state_);
}

void StaticScene::update(double delta)
{
    scene_state_.io_handler = io_handler_;
    scene_state_.delta = delta;

    // First, check for trigger zone collision directly
    // This needs to be done before the scene update to track zone entry/exit
    auto &camera = root_.get_child<0>();
    auto &witch_transform = camera.get_child<2>();
    auto &zone_transform = camera.get_child<3>();

    // Get collision components
    CircleCollisionComponent *witch_collider =
        static_cast<CircleCollisionComponent *>(witch_transform.get_collision_component());
    CircleCollisionComponent *zone_collider =
        static_cast<CircleCollisionComponent *>(zone_transform.get_collision_component());

    // Check for witch-zone collision
    bool is_colliding = false;
    if(witch_collider && zone_collider)
    {
        is_colliding = witch_collider->collides_with(*zone_collider);
    }

    // Handle witch-zone collision state
    handle_trigger_zone_collision(&witch_transform, is_colliding ? &zone_transform : nullptr);

    // Now handle general collisions using the collision system
    handle_collisions();

    // Update the scene graph
    root_.update(scene_state_);
}

void StaticScene::handle_collisions()
{
    // Get collision pairs from the system
    auto collision_pairs = collision_system_.check_collisions();

    // Get common scene nodes for reference
    auto &camera = root_.get_child<0>();
    auto &game_map_transform = camera.get_child<0>();

    // Handle each collision pair
    for(const auto &pair : collision_pairs)
    {
        // Get transform nodes
        TransformNode *transform_a = pair.first->get_owner();
        TransformNode *transform_b = pair.second->get_owner();

        // Handle boundary collisions (entities colliding with the map)
        if(transform_a == &game_map_transform)
        {
            handle_boundary_collision(transform_b, transform_a);
        }
        else if(transform_b == &game_map_transform)
        {
            handle_boundary_collision(transform_a, transform_b);
        }

        // Handle other collision types here
    }
}

void StaticScene::handle_boundary_collision(TransformNode *entity, TransformNode *boundary)
{
    // Basic boundary collision - just move back to previous position
    entity->set_position(entity->get_prev_position_x(), entity->get_prev_position_y());
}

void StaticScene::handle_trigger_zone_collision(TransformNode *witch, TransformNode *zone)
{
    if(witch == nullptr) return;

    auto &camera = root_.get_child<0>();
    auto &witch_transform_actual = camera.get_child<2>(); // Get the actual typed reference
    auto &witch_sprite = witch_transform_actual.get_child<0>();

    bool  is_colliding = (zone != nullptr);

    // Handle entering the zone
    if(is_colliding && !witch_in_zone_)
    {
        witch_in_zone_ = true;
        witch_zone_passes_++;

        std::cout << "CIRCLE-CIRCLE COLLISION: Witch entered trigger zone! Pass #"
                  << witch_zone_passes_ << std::endl;

        // Apply visual feedback based on zone passes
        witch_sprite.set_playback_speed(10.0f);
    }
    // Handle exiting the zone
    else if(!is_colliding && witch_in_zone_)
    {
        witch_in_zone_ = false;
        std::cout << "Witch exited trigger zone." << std::endl;

        // Reset visual effects
        witch_sprite.set_playback_speed(1.0f);
    }
}

// New collision management methods
void StaticScene::register_collision_component(CollisionComponent *component)
{
    collision_system_.add_component(component);
}



} // namespace cge
