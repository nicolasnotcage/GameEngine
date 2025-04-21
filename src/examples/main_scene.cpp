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

    // ------------------------------------
    //    Initialize Sprite Textures
    // ------------------------------------

    // Blue witch (NPC) walk texture
    blue_witch_run_texture_.set_filepath("images/blue_witch/B_witch_run.png");
    blue_witch_run_texture_.set_blend(true);
    blue_witch_run_texture_.set_blend_alpha(200);
    blue_witch_run_texture_.define_grid(1, 8, 32, 48);
    blue_witch_run_texture_.init(scene_state_);

    // Blue witch (NPC) idle texture
    blue_witch_idle_texture_.set_filepath("images/blue_witch/B_witch_idle.png");
    blue_witch_idle_texture_.set_blend(true);
    blue_witch_idle_texture_.set_blend_alpha(200);
    blue_witch_idle_texture_.define_grid(1, 6, 32, 48);
    blue_witch_idle_texture_.init(scene_state_);

    // Blue witch (NPC) transparent texture
    blue_witch_transparent_texture_.set_filepath("images/blue_witch/B_witch_transparent.png");
    blue_witch_transparent_texture_.set_blend(true);
    blue_witch_transparent_texture_.set_blend_alpha(0); // Fully transparent
    blue_witch_transparent_texture_.define_grid(1, 8, 32, 48);
    blue_witch_transparent_texture_.init(scene_state_);

    // White witch (Player) run texture
    white_witch_run_texture_.set_filepath("images/white_witch/witch_run.png");
    white_witch_run_texture_.set_blend(true);
    white_witch_run_texture_.set_blend_alpha(200);
    white_witch_run_texture_.define_grid(1, 6, 64, 64);
    white_witch_run_texture_.init(scene_state_);

    // White witch (Player) idle texture
    white_witch_idle_texture_.set_filepath("images/white_witch/witch_idle.png");
    white_witch_idle_texture_.set_blend(true);
    white_witch_idle_texture_.set_blend_alpha(200);
    white_witch_idle_texture_.define_grid(1, 6, 64, 64);
    white_witch_idle_texture_.init(scene_state_);

    // In-game text
    intro_1_.set_filepath("images/game_text/intro_1.png");
    intro_1_.set_blend(true);
    intro_1_.set_blend_alpha(200);
    intro_1_.init(scene_state_);

    intro_2_.set_filepath("images/game_text/intro_2.png");
    intro_2_.set_blend(true);
    intro_2_.set_blend_alpha(200);
    intro_2_.init(scene_state_);

    intro_3_.set_filepath("images/game_text/intro_3.png");
    intro_3_.set_blend(true);
    intro_3_.set_blend_alpha(200);
    intro_3_.init(scene_state_);

    intro_4_.set_filepath("images/game_text/intro_4.png");
    intro_4_.set_blend(true);
    intro_4_.set_blend_alpha(200);
    intro_4_.init(scene_state_);

    first_find_.set_filepath("images/game_text/first_find.png");
    first_find_.set_blend(true);
    first_find_.set_blend_alpha(200);
    first_find_.init(scene_state_);

    second_find_.set_filepath("images/game_text/second_find.png");
    second_find_.set_blend(true);
    second_find_.set_blend_alpha(200);
    second_find_.init(scene_state_);

    third_find_.set_filepath("images/game_text/third_find.png");
    third_find_.set_blend(true);
    third_find_.set_blend_alpha(200);
    third_find_.init(scene_state_);

    // Configure in-game text
    // Intro
    auto &intro_text_transform = root_.get_child<0>().get_child<2>().get_child<1>();
    auto &intro_text_node = intro_text_transform.get_child<0>();
    intro_text_transform.right_translate(0, 1.8f);
    intro_text_transform.right_scale(3.0f, 1.0f);
    intro_text_node.push_texture(&intro_1_);
    intro_text_node.push_texture(&intro_2_);
    intro_text_node.push_texture(&intro_3_);
    intro_text_node.push_texture(&intro_4_);
    intro_text_node.set_should_render(true);

    // First find
    auto& first_find_transform = root_.get_child<0>().get_child<2>().get_child<2>();
    auto& first_find_node = first_find_transform.get_child<0>();
    first_find_transform.right_translate(0, 1.8f);
    first_find_transform.right_scale(3.0f, 1.0f);
    first_find_node.push_texture(&first_find_);

    // Second find
    auto& second_find_transform = root_.get_child<0>().get_child<2>().get_child<3>();
    auto& second_find_node = second_find_transform.get_child<0>();
    second_find_transform.right_translate(0, 1.8f);
    second_find_transform.right_scale(3.0f, 1.0f);
    second_find_node.push_texture(&second_find_);

    // Third find
    auto& third_find_transform = root_.get_child<0>().get_child<2>().get_child<4>();
    auto& third_find_node = third_find_transform.get_child<0>();
    third_find_transform.right_translate(0, 1.8f);
    third_find_transform.right_scale(3.0f, 1.0f);
    third_find_node.push_texture(&third_find_);
    

    // ---------------------------
    //       Camera Setup
    // ---------------------------
    auto &camera = root_.get_child<0>();
    camera.get_camera().set_dimensions(20.0f, 15.0f);
    camera.get_camera().set_position(0.0f, 0.0f);

    // Set camera to print world coordinates on click; used for testing and defining collision boundaries
    camera.set_print_on_click(true);

    // ---------------------------
    //       Game Map Setup
    // ---------------------------
    
    // Get game map transform and texture nodes
    auto &game_map_transform = camera.get_child<0>();
    auto &game_map_tex = game_map_transform.get_child<0>();

    // Set map filepath and scale it
    game_map_tex.set_filepath("images/game_map.png");
    game_map_transform.right_scale(35.5f, 20.0f);

    // ------------------------
    //     Character Setup
    // ------------------------
    
    // Get character transforms
    auto &blue_witch_transform = camera.get_child<1>();
    auto &witch_transform = camera.get_child<2>();

    // Position blue witch and configure path
    blue_witch_transform.right_translate(4.0f, 1.0f); 
    blue_witch_path_.add_point(4.0f, 1.0f, 0.5f);  // Start position
    blue_witch_path_.add_point(4.0f, 2.5f, 0.5f);  // Move down
    blue_witch_path_.set_looping(false);
    blue_witch_transform.right_scale(2.0f, 2.0f);

    // Set witch transform if no save file exists
    if (!SaveManager::get_instance().save_exists()) witch_transform.right_translate(1.0f, 0.0f);

    // Scale witch
    witch_transform.right_scale(3.0f, 3.0f);

    // Set camera to follow player (witch)
    camera.set_target(&witch_transform, true);
    camera.set_follow_smoothness(1.0f);

    // Setup animations
    setup_npc_animations();
    setup_witch_animations();

    // Setup systems
    setup_collisions();
    setup_trigger_zones();
    
    // Setup audio first, before creating audio components
    setup_audio();
    
    // Add audio to golem with 3D settings
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

    // Modify test serializer values
    test_float_ += 0.1;
    test_int_ += 1;
    test_bool_ = !test_bool_;

    if (test_bool_) test_string_ = cge::utility::to_lower(test_string_);
    else test_string_ = cge::utility::to_upper(test_string_);

    // Initialize root node
    root_.init(scene_state_);
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
                    has_laughed_ = true;
                    time_to_laugh_ = 0.0f;

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

    auto& camera = root_.get_child<0>();
    auto& text_transform = camera.get_child<2>().get_child<1>();
    auto& text_node = text_transform.get_child<0>();
    auto& blue_witch_transform = camera.get_child<1>();
    auto& witch_transform = camera.get_child<2>();

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
                
                // Hide the NPC immediately
                auto& blue_witch_sprite = blue_witch_transform.get_child<0>();
                blue_witch_sprite.set_auto_animation_enabled(false);
                blue_witch_sprite.set_texture(&blue_witch_transparent_texture_);
                blue_witch_sprite.play("hidden");
                blue_witch_hidden_ = true;
                
                // Create a new path with the teleport location
                Path new_path{};
                new_path.add_point(14.6641f, 6.5282f, 0.0f);
                new_path.set_looping(false);
                
                // Set the path and teleport to next location
                blue_witch_transform.set_path_controlled(new_path);
                blue_witch_transform.set_position(14.6641f, 6.5282f);
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
                
                // Hide the NPC immediately
                auto& blue_witch_sprite = blue_witch_transform.get_child<0>();
                blue_witch_sprite.set_auto_animation_enabled(false);
                blue_witch_sprite.set_texture(&blue_witch_transparent_texture_);
                blue_witch_sprite.play("hidden");
                blue_witch_hidden_ = true;
                
                // Create a new path with the teleport location
                Path new_path{};
                new_path.add_point(3.3f, -2.525f, 0.0f);
                new_path.set_looping(false);
                
                // Set the path and teleport to next location
                blue_witch_transform.set_path_controlled(new_path);
                blue_witch_transform.set_position(3.3f, -2.525f);
            }
        }
        else if (find_count_ == 3) {
            // Third find dialogue
            auto& third_find_transform = camera.get_child<2>().get_child<4>();
            auto& third_find_node = third_find_transform.get_child<0>();
            dialogue_still_showing = third_find_node.is_rendered();
            
            // If dialogue is no longer showing, NPC should remain visible
            if (!dialogue_still_showing) {
                waiting_for_dialogue_ = false;
                
                // Make sure the NPC is visible and using the idle animation
                auto& blue_witch_sprite = blue_witch_transform.get_child<0>();
                blue_witch_sprite.set_auto_animation_enabled(true);
                blue_witch_sprite.set_texture(&blue_witch_idle_texture_);
                blue_witch_sprite.play("idle");
                blue_witch_hidden_ = false;
            }
        }
    }

    // Check if intro path is completed and witch should be hidden
    if (!blue_witch_hidden_ && !blue_witch_transform.is_moving() && 
        dialogue_completed_ && blue_witch_transform.get_position_y() >= 1.5f && 
        !waiting_for_dialogue_ && find_count_ == 0) 
    {
        // Set new pathing position based on find count
        Path new_path{};
        
        if (find_count_ == 1) {
            // First find - teleport to (14.6641, 6.5282)
            new_path.add_point(14.6641f, 6.5282f, 0.0f);
        } else if (find_count_ == 2) {
            // Second find - teleport to (3.3, -2.525)
            new_path.add_point(3.3f, -2.525f, 0.0f);
        } else {
            // Default position
            new_path.add_point(-15.55f, -7.625f, 0.0f);
        }
        
        new_path.set_looping(false);

        // Hide witch and move to new position
        auto& blue_witch_sprite = blue_witch_transform.get_child<0>();
        
        // Disable automatic animation switching to prevent overriding the hidden animation
        blue_witch_sprite.set_auto_animation_enabled(false);
        
        // Set the hidden animation and texture
        blue_witch_sprite.set_texture(&blue_witch_transparent_texture_);
        blue_witch_sprite.play("hidden");
        
        // Move the witch to the new position
        blue_witch_transform.set_path_controlled(new_path);
        
        if (find_count_ == 1) {
            blue_witch_transform.set_position(14.6641f, 6.5282f);
        } else if (find_count_ == 2) {
            blue_witch_transform.set_position(3.3f, -2.525f);
        } else {
            blue_witch_transform.set_position(-15.55f, -7.625f);
        }
        
        blue_witch_hidden_ = true;
    }
    
    // Special case for third find - NPC should remain visible
    if (find_count_ == 3 && waiting_for_dialogue_ == false && !blue_witch_transform.is_moving()) {
        // Make sure the NPC is visible and using the idle animation
        auto& blue_witch_sprite = blue_witch_transform.get_child<0>();
        
        // Re-enable automatic animation switching
        blue_witch_sprite.set_auto_animation_enabled(true);
        
        // Set the idle animation and texture
        blue_witch_sprite.set_texture(&blue_witch_idle_texture_);
        blue_witch_sprite.play("idle");
        
        // Mark witch as visible
        blue_witch_hidden_ = false;
    }
    
    // Check for pause action to open pause menu
    const GameActionList &actions = io_handler_->get_game_actions();
    for (uint8_t i = 0; i < actions.num_actions; i++)
    {
        if (actions.actions[i] == GameAction::TOGGLE_PAUSE)
        {
            // Push the pause menu
            SceneManager::get_instance()->push_scene_by_key("pause_menu");
            return; // Exit early to prevent further updates this frame
        }
    }

    // Handle general collisions using the collision system
    handle_collisions();
    handle_audio();

    // Update the scene graph
    root_.update(scene_state_);
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
    
    // Update listener position (player position) first
    Vector2 witch_position(witch_transform.get_position_x(), witch_transform.get_position_y());
    AudioEngine::get_instance()->set_3d_listener_position(witch_position);
    
    // Update NPC's audio component position for 3D audio
    if (auto* blue_witch_audio = blue_witch_transform.get_audio_component())
    {
        // Make sure 3D position is updated
        blue_witch_audio->update_position();
        
        // Configure 3D audio parameters - using smaller values for better effect
        blue_witch_audio->set_min_distance(1.0f);  // Closer min distance for more pronounced effect
        blue_witch_audio->set_max_distance(10.0f); // Shorter max distance for more noticeable falloff
    }    

    // Update clap timer if waiting to clap
    if (waiting_to_clap_) 
    {
        npc_audio_timer_ += scene_state_.delta;
        
        // Check if it's time to clap
        if (npc_audio_timer_ >= time_to_clap_) 
        {
            // Play the clap sound
            if (auto* blue_witch_audio = blue_witch_transform.get_audio_component()) 
            {
                if (blue_witch_hidden_) blue_witch_audio->play(1.0f);
            }
            
            // Reset the timer and flag
            waiting_to_clap_ = false;
            npc_audio_timer_ = 0.0f;
        }
    }

    // Check for whistling action
    const GameActionList &actions = io_handler_->get_game_actions();
    for(uint8_t i = 0; i < actions.num_actions; i++)
    {
        if (actions.actions[i] == GameAction::PLAYER_WHISTLE) {
            // Play witch whistle sound
            if (auto* witch_audio = witch_transform.get_audio_component()) {
                witch_audio->play(1.0f);
            }

            // Set flag to clap after delay if hidden
            if (blue_witch_hidden_ && !waiting_to_clap_) 
            {
                waiting_to_clap_ = true;
                npc_audio_timer_ = 0.0f;
            }
            break;
        }
        
        else if (actions.actions[i] == GameAction::TOGGLE_MUSIC) 
        {
            // Toggle music
            AudioEngine::get_instance()->toggle_music();
            break;
        }
    }
    
    // No need to call update() here as it's called after position changes
    // in set_3d_listener_position and update_position
}

// Serialization overrides
void MainScene::serialize(Serializer& serializer) const
{
    // Serialize player (witch) position
    auto &camera = root_.get_child<0>();
    auto &witch_transform = camera.get_child<2>();

    // Directly write position to make sure it's saved with consistent keys
    float player_x = witch_transform.get_position_x();
    float player_y = witch_transform.get_position_y();
    serializer.write("player_x", player_x);
    serializer.write("player_y", player_y);

    // Serialize test values
    serializer.write("test_float", test_float_);
    serializer.write("test_int", test_int_);
    serializer.write("test_bool", test_bool_);
    serializer.write("test_string", test_string_);
}

void MainScene::deserialize(Serializer& serializer)
{    
    // Deserialize player (witch) position
    auto &camera = root_.get_child<0>();
    auto &witch_transform = camera.get_child<2>();
    float player_x = 0.0f, player_y = 0.0f;
    if (serializer.read("player_x", player_x) && serializer.read("player_y", player_y)) 
    {          
        witch_transform.set_position(player_x, player_y);
    }

    // Serialize test values
    serializer.read("test_float", test_float_);
    serializer.read("test_int", test_int_);
    serializer.read("test_bool", test_bool_);
    serializer.read("test_string", test_string_);
}

} // namespace cge
