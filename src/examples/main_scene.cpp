/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/main_scene.hpp"
#include "platform/animation.hpp"
#include "platform/audio_engine.hpp"
#include "platform/collision_system.hpp"
#include "platform/scene_manager.hpp"

#include "system/file_locator.hpp"
#include "system/serializer.hpp"
#include "system/save_manager.hpp"
#include "system/config_manager.hpp"
#include "system/string_utils.hpp"

#include <iostream>

namespace cge
{

void MainScene::init(SDLInfo *sdl_info, IoHandler *io_handler)
{
    sdl_info_ = sdl_info;
    io_handler_ = io_handler;

    SDL_SetRenderDrawColor(sdl_info->renderer, 0, 0, 0, 0);
    SDL_SetRenderDrawBlendMode(sdl_info->renderer, SDL_BLENDMODE_BLEND);

    // Reset scene state
    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;
    scene_state_.io_handler = io_handler_;

    // Initialize textures
    initialize_character_textures();
    initialize_dialogue_textures();

    // Setup scene components
    setup_camera();
    setup_game_map();
    setup_characters();
    setup_dialogue_nodes();

    // Setup animations
    setup_npc_animations();
    setup_witch_animations();

    // Setup systems
    setup_collisions();
    setup_trigger_zones();
    setup_audio();
    
    // Add audio components to characters
    auto &camera = root_.get_child<0>();
    auto &blue_witch_transform = camera.get_child<1>();
    auto &witch_transform = camera.get_child<2>();
    
    // Add audio to blue witch with 3D settings
    auto *blue_witch_audio = blue_witch_transform.add_audio_component();
    blue_witch_audio->set_sound("npc_clap");
    blue_witch_audio->set_min_distance(1.0f);
    blue_witch_audio->set_max_distance(10.0f);

    // Add audio to witch
    auto *witch_audio = witch_transform.add_audio_component();
    witch_audio->set_sound("whistle");

    // Begin theme music at low volume
    AudioEngine::get_instance()->play_sound("theme_music", 0.2f);

    // Mute theme music if configuration set
    bool music_enabled = ConfigManager::get_instance().get_music_enabled();
    if (!music_enabled) AudioEngine::get_instance()->get_channel(3)->setMute(true);

    // Initialize root node
    root_.init(scene_state_);
}

// Helper method to configure a texture with common settings
void MainScene::configure_texture(TextureNode& texture, const std::string& filepath, bool blend, int alpha, int rows, int cols, int width, int height)
{
    texture.set_filepath(filepath);
    texture.set_blend(blend);
    texture.set_blend_alpha(alpha);
    
    if (cols > 1 || rows > 1) {
        texture.define_grid(rows, cols, width, height);
    }
    
    texture.init(scene_state_);
}

// Initialize character textures
void MainScene::initialize_character_textures()
{
    // Blue witch (NPC) textures
    configure_texture(blue_witch_run_texture_, "images/blue_witch/B_witch_run.png", true, 200, 1, 8, 32, 48);
    configure_texture(blue_witch_idle_texture_, "images/blue_witch/B_witch_idle.png", true, 200, 1, 6, 32, 48);
    configure_texture(blue_witch_transparent_texture_, "images/blue_witch/B_witch_transparent.png", true, 0, 1, 8, 32, 48);
    
    // White witch (Player) textures
    configure_texture(white_witch_run_texture_, "images/white_witch/witch_run.png", true, 200, 1, 6, 64, 64);
    configure_texture(white_witch_idle_texture_, "images/white_witch/witch_idle.png", true, 200, 1, 6, 64, 64);
}

// Initialize dialogue textures
void MainScene::initialize_dialogue_textures()
{
    // Intro dialogue textures
    configure_texture(intro_1_, "images/game_text/intro_1.png", true, 200);
    configure_texture(intro_2_, "images/game_text/intro_2.png", true, 200);
    configure_texture(intro_3_, "images/game_text/intro_3.png", true, 200);
    configure_texture(intro_4_, "images/game_text/intro_4.png", true, 200);
    configure_texture(intro_5_, "images/game_text/intro_5.png", true, 200);
    
    // Find dialogue textures
    configure_texture(first_find_, "images/game_text/first_find.png", true, 200);
    configure_texture(second_find_, "images/game_text/second_find.png", true, 200);
    configure_texture(third_find_, "images/game_text/third_find.png", true, 200);
}

// Setup camera
void MainScene::setup_camera()
{
    auto &camera = root_.get_child<0>();
    camera.get_camera().set_dimensions(20.0f, 15.0f);
    camera.get_camera().set_position(0.0f, 0.0f);

    // Set camera to print world coordinates on click; used for testing and defining collision boundaries
    camera.set_print_on_click(true);
}

// Setup game map
void MainScene::setup_game_map()
{
    auto &camera = root_.get_child<0>();
    auto &game_map_transform = camera.get_child<0>();
    auto &game_map_tex = game_map_transform.get_child<0>();

    // Set map filepath and scale it
    game_map_tex.set_filepath("images/game_map.png");
    game_map_transform.right_scale(35.5f, 20.0f);
}

// Setup characters
void MainScene::setup_characters()
{
    // Get nodes
    auto &camera = root_.get_child<0>();
    auto &blue_witch_transform = camera.get_child<1>();
    auto &blue_witch_sprite = blue_witch_transform.get_child<0>();
    auto &witch_transform = camera.get_child<2>();
    auto &witch_sprite = witch_transform.get_child<0>();

    // Create character objects
    blue_witch_ = std::make_unique<NPC>(&blue_witch_transform, &blue_witch_sprite);
    player_ = std::make_unique<Player>(&witch_transform, &witch_sprite);

    // Configure NPC with textures
    blue_witch_->set_hidden_texture(&blue_witch_transparent_texture_);
    blue_witch_->set_idle_texture(&blue_witch_idle_texture_);

    // Configure Player with textures
    player_->set_run_texture(&white_witch_run_texture_);
    player_->set_idle_texture(&white_witch_idle_texture_);

    // Position blue witch and configure path
    blue_witch_->set_position(4.0f, 1.0f);
    blue_witch_path_.add_point(4.0f, 1.0f, 0.5f);  // Start position
    blue_witch_path_.add_point(4.0f, 2.5f, 0.5f);  // Move down
    blue_witch_path_.set_looping(false);

    // Scale characters
    blue_witch_transform.right_scale(2.0f, 2.0f);
    witch_transform.right_scale(3.0f, 3.0f);

    // Set player position if no save file exists
    if (!SaveManager::get_instance().save_exists()) player_->set_position(1.0f, 0.0f);

    // Set camera to follow player (witch)
    camera.set_target(&witch_transform, true);
    camera.set_follow_smoothness(1.0f);
}

// Configure a dialogue text node
void MainScene::configure_dialogue_text_node(TransformNode& transform_node, TextNode& text_node, TextureNode* texture)
{
    transform_node.right_translate(0, 1.8f);
    transform_node.right_scale(3.0f, 1.0f);
    
    if (texture) {
        text_node.push_texture(texture);
    }
}

// Setup dialogue nodes
void MainScene::setup_dialogue_nodes()
{
    auto &camera = root_.get_child<0>();
    
    // Intro text
    auto &intro_text_transform = camera.get_child<2>().get_child<1>();
    auto &intro_text_node = intro_text_transform.get_child<0>();
    configure_dialogue_text_node(intro_text_transform, intro_text_node, nullptr);
    
    // Add all intro textures
    intro_text_node.push_texture(&intro_1_);
    intro_text_node.push_texture(&intro_2_);
    intro_text_node.push_texture(&intro_3_);
    intro_text_node.push_texture(&intro_4_);
    intro_text_node.push_texture(&intro_5_);

    // Only render intro text on fresh game instance (if dialogue not completed)
    intro_text_node.set_should_render(!dialogue_completed_);

    // First find
    auto& first_find_transform = camera.get_child<2>().get_child<2>();
    auto& first_find_node = first_find_transform.get_child<0>();
    configure_dialogue_text_node(first_find_transform, first_find_node, &first_find_);

    // Second find
    auto& second_find_transform = camera.get_child<2>().get_child<3>();
    auto& second_find_node = second_find_transform.get_child<0>();
    configure_dialogue_text_node(second_find_transform, second_find_node, &second_find_);

    // Third find
    auto& third_find_transform = camera.get_child<2>().get_child<4>();
    auto& third_find_node = third_find_transform.get_child<0>();
    configure_dialogue_text_node(third_find_transform, third_find_node, &third_find_);
}

void MainScene::setup_npc_animations()
{
    auto &camera = root_.get_child<0>();
    auto &blue_witch_transform = camera.get_child<1>();
    auto &blue_witch_sprite = blue_witch_transform.get_child<0>();

    // Create walk animation
    Animation run_animation("run", true);
    for(int i = 0; i < 8; i++) { run_animation.add_frame(i, 10); }

    // Create idle animation
    Animation idle_animation("idle", true);
    for(int i = 0; i < 6; i++) { idle_animation.add_frame(i, 10); }

    // Create hidden animation
    Animation hidden_animation("hidden", true);
    for (int i = 0; i < 8; i++) { hidden_animation.add_frame(i, 10); }

    // Add animations with their respective textures
    blue_witch_sprite.add_animation_with_texture(run_animation, &blue_witch_run_texture_);
    blue_witch_sprite.add_animation_with_texture(idle_animation, &blue_witch_idle_texture_);
    blue_witch_sprite.add_animation_with_texture(hidden_animation, &blue_witch_transparent_texture_);

    // Set initial texture
    blue_witch_sprite.set_texture(&blue_witch_idle_texture_);

    // Start playing idle animation by default
    blue_witch_sprite.play("idle");

    // Associate the golem's transform with its sprite
    blue_witch_transform.set_associated_sprite(&blue_witch_sprite);
}

void MainScene::setup_witch_animations()
{
    auto &camera = root_.get_child<0>();
    auto &witch_transform = camera.get_child<2>();
    auto &witch_sprite = witch_transform.get_child<0>();

    // Create run animation
    Animation run_animation("run", true);
    for(int i = 0; i < 6; i++) { run_animation.add_frame(i, 10); }

    // Create idle animation
    Animation idle_animation("idle", true);
    for(int i = 0; i < 6; i++) { idle_animation.add_frame(i, 10); }

    // Add animations with their respective textures
    witch_sprite.add_animation_with_texture(run_animation, &white_witch_run_texture_);
    witch_sprite.add_animation_with_texture(idle_animation, &white_witch_idle_texture_);

    // Set initial texture
    witch_sprite.set_texture(&white_witch_idle_texture_);

    // Start playing run animation by default
    witch_sprite.play("idle");

    // Set witch as player controlled and associate its transform with its sprite
    witch_transform.set_player_controlled();
    witch_transform.set_associated_sprite(&witch_sprite);
}

void MainScene::setup_collisions()
{
    // Initialize boundary nodes (doesn't do anything currently but in case updates are made to texture node in the future)
    bottom_boundary_.init(scene_state_);
    top_boundary_.init(scene_state_);
    left_boundary_.init(scene_state_);
    right_boundary_.init(scene_state_);
    left_pillar_.init(scene_state_);
    right_pillar_.init(scene_state_);

    // Configure world boundaries
    auto bottom = bottom_boundary_.add_aabb_collider(Vector2(-17.3, 8.5), Vector2(17.75, 9.88));
    auto top = top_boundary_.add_aabb_collider(Vector2(-17.3, -10.4), Vector2(17.75, -10.2));
    auto left = left_boundary_.add_aabb_collider(Vector2(-18.3, -9.65), Vector2(-17.5, 9.88));
    auto right = right_boundary_.add_aabb_collider(Vector2(17.6, -9.65), Vector2(17.75, 9.88));
    auto pillar_left = left_pillar_.add_aabb_collider(Vector2(-12.9, -5.5), Vector2(-8.4, -1.3));
    auto pillar_right = right_pillar_.add_aabb_collider(Vector2(8.4, -5.5), Vector2(13.0, -1.3));
    
    // Character objects
    auto& camera = root_.get_child<0>();
    auto& blue_witch_transform = camera.get_child<1>();
    auto& witch_transform = camera.get_child<2>();

    // Add AABB collision component to witch (player)
    auto witch_collider =
        witch_transform.add_aabb_collider(Vector2(-0.5f, -1.0f), Vector2(0.5f, 1.0f));

    // Add AABB collision component to blue witch (NPC)
    auto blue_witch_collider =
        blue_witch_transform.add_aabb_collider(Vector2(-0.5f, -1.0f), Vector2(0.5f, 1.0f));

    // Register components with the collision system
    collision_system_.add_component(bottom, CollisionSystem::CollisionType::BOUNDARY);
    collision_system_.add_component(left, CollisionSystem::CollisionType::BOUNDARY);
    collision_system_.add_component(right, CollisionSystem::CollisionType::BOUNDARY);
    collision_system_.add_component(top, CollisionSystem::CollisionType::BOUNDARY);
    collision_system_.add_component(pillar_left, CollisionSystem::CollisionType::BOUNDARY);
    collision_system_.add_component(pillar_right, CollisionSystem::CollisionType::BOUNDARY);
    collision_system_.add_component(blue_witch_collider, CollisionSystem::CollisionType::ENTITY);
    collision_system_.add_component(witch_collider, CollisionSystem::CollisionType::ENTITY);
    
    // Register boundary collision response
    collision_system_.register_boundary_response([this](TransformNode* entity, TransformNode* boundary) 
        {
        handle_boundary_collision(entity, boundary);
        });

    // Register entity collision response for witches collision
    collision_system_.register_entity_response([this, &blue_witch_transform, &witch_transform](TransformNode* entity_a, TransformNode* entity_b) {
        // Check for witches collision
        bool witches_colliding =
            (entity_a == &witch_transform && entity_b == &blue_witch_transform) ||
            (entity_b == &witch_transform && entity_a == &blue_witch_transform);

        // Handle witch found logic
        if (witches_colliding && blue_witch_hidden_)
        {
            // Reveal witch if player investigates
            const GameActionList& actions = io_handler_->get_game_actions();
            for (uint8_t i = 0; i < actions.num_actions; i++)
            {
                if (actions.actions[i] == GameAction::INVESTIGATE)
                {
                    // Make blue witch visible again
                    auto& blue_witch_sprite = blue_witch_transform.get_child<0>();

                    // Re-enable automatic animation switching
                    blue_witch_sprite.set_auto_animation_enabled(true);

                    // Set the idle animation and texture
                    blue_witch_sprite.set_texture(&blue_witch_idle_texture_);
                    blue_witch_sprite.play("idle");

                    // Play found sound
                    AudioEngine::get_instance()->play_sound("success", 1.0f);

                    // Mark witch as found
                    blue_witch_hidden_ = false;

                    // Get camera reference for text nodes
                    auto& camera = root_.get_child<0>();

                    // Show appropriate text node based on find count
                    if (find_count_ == 0) {
                        // First find
                        auto& text_transform = camera.get_child<2>().get_child<2>();
                        auto& text_node = text_transform.get_child<0>();
                        text_node.set_should_render(true);
                    } else if (find_count_ == 1) {
                        // Second find
                        auto& text_transform = camera.get_child<2>().get_child<3>();
                        auto& text_node = text_transform.get_child<0>();
                        text_node.set_should_render(true);
                    } else if (find_count_ == 2) {
                        // Third find - NPC should remain visible after this
                        auto& text_transform = camera.get_child<2>().get_child<4>();
                        auto& text_node = text_transform.get_child<0>();
                        text_node.set_should_render(true);
                    }

                    // Set waiting for dialogue flag
                    waiting_for_dialogue_ = true;

                    // Increment find count
                    find_count_++;
                    
                    // Check if the game is completed (found NPC three times)
                    if (find_count_ == 3) {
                        // Set a flag to show game over menu after dialogue completes
                        game_completed_ = true;
                    }
                }
            }
        }
        });
}

void MainScene::setup_trigger_zones()
{
    // Configure trigger zones here
}

void MainScene::setup_audio() 
{ 
    cge::AudioEngine *audio_engine = cge::AudioEngine::get_instance(); 
    
    // Locate files
    auto player_file_info = locate_path_for_filename("audio/whistle.wav");
    auto npc_file_info = locate_path_for_filename("audio/npc_clap.wav");
    auto collision_sound_info = locate_path_for_filename("audio/success.wav");
    auto theme_sound_info = locate_path_for_filename("audio/theme_music.mp3");

    // Load sounds - note that npc_clap is now a 3D sound
    audio_engine->load_sound(player_file_info.path, "whistle", false, false);
    audio_engine->load_sound(npc_file_info.path, "npc_clap", true, false); // Set as 3D sound
    audio_engine->load_sound(collision_sound_info.path, "success", false, false);
    audio_engine->load_sound(theme_sound_info.path, "theme_music", false, true);


    // Reserve channels for each sound
    audio_engine->reserve_channel_for_sound("success", 0);
    audio_engine->reserve_channel_for_sound("npc_clap", 1);
    audio_engine->reserve_channel_for_sound("whistle", 2);
    audio_engine->reserve_channel_for_sound("theme_music", 3);
}

void MainScene::destroy()
{
    root_.destroy();
    white_witch_idle_texture_.destroy();
    white_witch_run_texture_.destroy();
    blue_witch_run_texture_.destroy();
    blue_witch_idle_texture_.destroy();
    blue_witch_transparent_texture_.destroy(); // Add this line
    intro_1_.destroy();
    intro_2_.destroy();
    intro_3_.destroy();
    intro_4_.destroy();
    bottom_boundary_.destroy();
    top_boundary_.destroy();
    left_boundary_.destroy();
    right_boundary_.destroy();
    left_pillar_.destroy();
    right_pillar_.destroy();
}

void MainScene::render()
{
    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;
    scene_state_.io_handler = io_handler_;

    root_.draw(scene_state_);
}

void MainScene::update(double delta)
{
    scene_state_.io_handler = io_handler_;
    scene_state_.delta = delta;

    // Update player and NPC objects
    player_->update(delta);
    blue_witch_->update(delta);

    // Handle dialogue and NPC state
    handle_dialogue_state();
    handle_npc_state();
    
    // Handle input actions (pause, etc.)
    handle_input_actions();

    // Handle general collisions using the collision system
    handle_collisions();
    handle_audio();

    // Update the scene graph
    root_.update(scene_state_);
}

// Handle dialogue state changes
void MainScene::handle_dialogue_state()
{
    auto& camera = root_.get_child<0>();
    auto& text_transform = camera.get_child<2>().get_child<1>();
    auto& text_node = text_transform.get_child<0>();
    auto& blue_witch_transform = camera.get_child<1>();

    // Check if intro dialogue is not rendered anymore (completed)
    if (!dialogue_completed_ && !text_node.is_rendered()) 
    {
        dialogue_completed_ = true;
        blue_witch_transform.set_path_controlled(blue_witch_path_);
    }

    // Check if find dialogue is not rendered anymore (completed)
    if (waiting_for_dialogue_) 
    {
        bool dialogue_still_showing = false;
        
        // Check which dialogue is currently showing based on find_count_
        if (find_count_ == 1) {
            // First find dialogue
            auto& first_find_transform = camera.get_child<2>().get_child<2>();
            auto& first_find_node = first_find_transform.get_child<0>();
            dialogue_still_showing = first_find_node.is_rendered();
            
            // If dialogue is no longer showing, teleport NPC to next location
            if (!dialogue_still_showing) {
                waiting_for_dialogue_ = false;
                hide_npc();
                teleport_npc_to_location(14.6641f, 6.5282f);
            }
        } 
        else if (find_count_ == 2) {
            // Second find dialogue
            auto& second_find_transform = camera.get_child<2>().get_child<3>();
            auto& second_find_node = second_find_transform.get_child<0>();
            dialogue_still_showing = second_find_node.is_rendered();
            
            // If dialogue is no longer showing, teleport NPC to next location
            if (!dialogue_still_showing) {
                waiting_for_dialogue_ = false;
                hide_npc();
                teleport_npc_to_location(3.3f, -2.525f);
            }
        }
        else if (find_count_ == 3) {
            // Third find dialogue
            auto& third_find_transform = camera.get_child<2>().get_child<4>();
            auto& third_find_node = third_find_transform.get_child<0>();
            dialogue_still_showing = third_find_node.is_rendered();
            
            // If dialogue is no longer showing, NPC should remain visible
            if (!dialogue_still_showing) 
            {
                waiting_for_dialogue_ = false;
                show_npc();
                
                // If the game is completed, show the game over menu
                if (game_completed_) 
                {
                    // Push the game over menu
                    SceneManager::get_instance()->push_scene_by_key("game_over_menu");
                }
            }
        }
    }
}

// Handle NPC state changes
void MainScene::handle_npc_state()
{
    auto& camera = root_.get_child<0>();
    auto& blue_witch_transform = camera.get_child<1>();

    // Check if intro path is completed and witch should be hidden
    if (!blue_witch_hidden_ && !blue_witch_transform.is_moving() && 
        dialogue_completed_ && blue_witch_transform.get_position_y() >= 1.5f && 
        !waiting_for_dialogue_ && find_count_ == 0) 
    {
        // Hide witch and move to new position
        hide_npc();
        
        // Set position based on find count
        if (find_count_ == 1) {
            teleport_npc_to_location(14.6641f, 6.5282f);
        } else if (find_count_ == 2) {
            teleport_npc_to_location(3.3f, -2.525f);
        } else {
            teleport_npc_to_location(-15.55f, -7.625f);
        }
    }
    
    // Special case for third find - NPC should remain visible
    if (find_count_ == 3 && waiting_for_dialogue_ == false && !blue_witch_transform.is_moving()) {
        show_npc();
    }
}

// Teleport NPC to a specific location
void MainScene::teleport_npc_to_location(float x, float y)
{
    // Delegate to NPC's teleport method
    blue_witch_->teleport_to(x, y);
}

// Hide NPC
void MainScene::hide_npc()
{
    // Delegate to NPC's hide method
    blue_witch_->hide();
    
    // Update the scene's state flag
    blue_witch_hidden_ = true;
}

// Show NPC
void MainScene::show_npc()
{
    // Delegate to NPC's show method
    blue_witch_->show();
    
    // Update the scene's state flag
    blue_witch_hidden_ = false;
}

// Show dialogue for a specific find
void MainScene::show_dialogue_for_find(int find_number)
{
    auto& camera = root_.get_child<0>();
    
    if (find_number == 1) {
        // First find
        auto& text_transform = camera.get_child<2>().get_child<2>();
        auto& text_node = text_transform.get_child<0>();
        text_node.set_should_render(true);
    } else if (find_number == 2) {
        // Second find
        auto& text_transform = camera.get_child<2>().get_child<3>();
        auto& text_node = text_transform.get_child<0>();
        text_node.set_should_render(true);
    } else if (find_number == 3) {
        // Third find
        auto& text_transform = camera.get_child<2>().get_child<4>();
        auto& text_node = text_transform.get_child<0>();
        text_node.set_should_render(true);
    }
}

// Handle input actions
void MainScene::handle_input_actions()
{
    // Delegate to player's input handling
    player_->handle_input_actions(io_handler_);
}

void MainScene::handle_collisions()
{  
    // Process all collisions using the collision system
    collision_system_.process_collisions();
}

void MainScene::handle_boundary_collision(TransformNode *entity, TransformNode *boundary)
{
    // Basic boundary collision. Just moves the player back to their previous position.
    entity->set_position(entity->get_prev_position_x(), entity->get_prev_position_y());
}

// Register a collision component with the collision system. 
void MainScene::register_collision_component(std::shared_ptr<CollisionComponent> component)
{
    collision_system_.add_component(component, CollisionSystem::CollisionType::ENTITY);
}

void MainScene::handle_audio()
{    
    auto &camera = root_.get_child<0>();
    auto &blue_witch_transform = camera.get_child<1>();
    auto &witch_transform = camera.get_child<2>();
    
    // Update 3D audio positioning
    update_audio_positions(witch_transform, blue_witch_transform);
    
    // Handle NPC audio timing
    update_npc_audio_timing(blue_witch_transform);
    
    // Process audio-related input actions
    process_audio_actions(witch_transform);
}

// Update 3D audio positioning for listener and sources
void MainScene::update_audio_positions(TransformNode& player_transform, TransformNode& npc_transform)
{
    // Update listener position (player position) first
    Vector2 player_position(player_transform.get_position_x(), player_transform.get_position_y());
    AudioEngine::get_instance()->set_3d_listener_position(player_position);
    
    // Update NPC's audio component position for 3D audio
    if (auto* npc_audio = npc_transform.get_audio_component())
    {
        // Make sure 3D position is updated
        npc_audio->update_position();
        
        // Configure 3D audio parameters - using smaller values for better effect
        npc_audio->set_min_distance(1.0f);  // Closer min distance for more pronounced effect
        npc_audio->set_max_distance(10.0f); // Shorter max distance for more noticeable falloff
    }
}

// Update NPC audio timing for delayed sounds
void MainScene::update_npc_audio_timing(TransformNode& npc_transform)
{
    // This method is now a no-op as the NPC class handles its own audio timing
    // The NPC's update method calls process_audio_timing internally
}

// Process audio-related input actions
void MainScene::process_audio_actions(TransformNode& player_transform)
{
    // Delegate to player's audio action handling
    player_->process_audio_actions(io_handler_, blue_witch_.get());
}

// Serialization overrides
void MainScene::serialize(Serializer& serializer) const
{
    // Serialize player (witch) position
    auto &camera = root_.get_child<0>();
    auto& blue_witch_transform = camera.get_child<1>();
    auto &witch_transform = camera.get_child<2>();

    // Directly write position to make sure it's saved with consistent keys
    float player_x = witch_transform.get_position_x();
    float player_y = witch_transform.get_position_y();
    float npc_x = blue_witch_transform.get_position_x();
    float npc_y = blue_witch_transform.get_position_y();

    serializer.write("player_x", player_x);
    serializer.write("player_y", player_y);
    serializer.write("npc_x", npc_x);
    serializer.write("npc_y", npc_y);
    serializer.write("waiting_to_clap", waiting_to_clap_);
    serializer.write("dialogue_completed", dialogue_completed_);
    serializer.write("blue_witch_hidden", blue_witch_hidden_);
    serializer.write("find_count", find_count_);
    serializer.write("waiting_for_dialogue", waiting_for_dialogue_);
    serializer.write("game_completed", game_completed_);
}

void MainScene::deserialize(Serializer& serializer)
{    
    // First check if the game was completed
    bool game_completed = false;
    serializer.read("game_completed", game_completed);
    
    // If the game was completed, don't load the saved state
    // This forces a new game instance if a player tries to load a completed game
    if (game_completed) {
        std::cout << "Loading a completed game - starting a new game instead." << std::endl;
        return;
    }
    
    // Deserialize player (witch) position
    auto &camera = root_.get_child<0>();
    auto &witch_transform = camera.get_child<2>();
    auto &blue_witch_transform = camera.get_child<1>();
    
    float player_x = 0.0f, player_y = 0.0f;
    float npc_x = 0.0f, npc_y = 0.0f;
    
    // Load player position
    if (serializer.read("player_x", player_x) && serializer.read("player_y", player_y)) 
    {          
        witch_transform.set_position(player_x, player_y);
    }
    
    // Load NPC position
    if (serializer.read("npc_x", npc_x) && serializer.read("npc_y", npc_y)) 
    {          
        blue_witch_transform.set_position(npc_x, npc_y);
    }
    
    // Load game state flags
    serializer.read("waiting_to_clap", waiting_to_clap_);
    serializer.read("dialogue_completed", dialogue_completed_);
    serializer.read("blue_witch_hidden", blue_witch_hidden_);
    serializer.read("find_count", find_count_);
    serializer.read("waiting_for_dialogue", waiting_for_dialogue_);
    serializer.read("game_completed", game_completed_);
    
    // Update NPC visibility based on loaded state
    if (blue_witch_hidden_) {
        hide_npc();
    } else {
        show_npc();
    }
    
    // Update dialogue state based on loaded state
    if (waiting_for_dialogue_ && find_count_ > 0 && find_count_ <= 3) {
        show_dialogue_for_find(find_count_);
    }
    
    // Hide intro text if dialogue is already completed
    if (dialogue_completed_) 
    {
        auto& intro_text_transform = camera.get_child<2>().get_child<1>();
        auto& intro_text_node = intro_text_transform.get_child<0>();
        intro_text_node.set_should_render(false);
    }
}

} // namespace cge
