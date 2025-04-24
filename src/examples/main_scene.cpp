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

    // Initialize character animations
    player_->init_animations();
    blue_witch_->init_animations();

    // Setup systems
    setup_collisions();
    setup_audio();

    // Initialize root node
    root_.init(scene_state_);
}

// Helper method to configure a texture with common settings
void MainScene::configure_texture(TextureNode& texture, const std::string& filepath, bool blend, int alpha, int rows, int cols, int width, int height)
{
    texture.set_filepath(filepath);
    texture.set_blend(blend);
    texture.set_blend_alpha(alpha);
    
    if (cols > 1 || rows > 1) texture.define_grid(rows, cols, width, height);

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
    //camera.set_print_on_click(true);
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

    // Create character objects with shared pointers
    blue_witch_ = std::make_shared<NPC>(&blue_witch_transform, &blue_witch_sprite);
    player_ = std::make_shared<Player>(&witch_transform, &witch_sprite);

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
    
    if (texture) text_node.push_texture(texture);
}

// Setup dialogue nodes
void MainScene::setup_dialogue_nodes()
{
    auto &camera = root_.get_child<0>();
    
    // Create dialogue manager
    dialogue_manager_ = std::make_unique<DialogueManager>();
    dialogue_manager_->init(scene_state_);
    
    // Register dialogue textures
    dialogue_manager_->register_texture("intro_1", &intro_1_);
    dialogue_manager_->register_texture("intro_2", &intro_2_);
    dialogue_manager_->register_texture("intro_3", &intro_3_);
    dialogue_manager_->register_texture("intro_4", &intro_4_);
    dialogue_manager_->register_texture("intro_5", &intro_5_);
    dialogue_manager_->register_texture("first_find", &first_find_);
    dialogue_manager_->register_texture("second_find", &second_find_);
    dialogue_manager_->register_texture("third_find", &third_find_);
    
    // Configure and register dialogue nodes
    auto &intro_text_transform = camera.get_child<2>().get_child<1>();
    auto &intro_text_node = intro_text_transform.get_child<0>();
    configure_dialogue_text_node(intro_text_transform, intro_text_node, nullptr);
    dialogue_manager_->register_text_node("intro", &intro_text_node);
    
    // Add all intro textures
    intro_text_node.push_texture(&intro_1_);
    intro_text_node.push_texture(&intro_2_);
    intro_text_node.push_texture(&intro_3_);
    intro_text_node.push_texture(&intro_4_);
    intro_text_node.push_texture(&intro_5_);

    // First find
    auto& first_find_transform = camera.get_child<2>().get_child<2>();
    auto& first_find_node = first_find_transform.get_child<0>();
    configure_dialogue_text_node(first_find_transform, first_find_node, &first_find_);
    dialogue_manager_->register_text_node("first_find", &first_find_node);

    // Second find
    auto& second_find_transform = camera.get_child<2>().get_child<3>();
    auto& second_find_node = second_find_transform.get_child<0>();
    configure_dialogue_text_node(second_find_transform, second_find_node, &second_find_);
    dialogue_manager_->register_text_node("second_find", &second_find_node);

    // Third find
    auto& third_find_transform = camera.get_child<2>().get_child<4>();
    auto& third_find_node = third_find_transform.get_child<0>();
    configure_dialogue_text_node(third_find_transform, third_find_node, &third_find_);
    dialogue_manager_->register_text_node("third_find", &third_find_node);
    
    // Register dialogue completion callback
    dialogue_manager_->register_on_dialogue_completed_callback(
        [this](DialogueManager::DialogueState state) 
        {
            handle_dialogue_completed(state);
        });
    
    // Show intro dialogue if not completed
    if (!dialogue_completed_) dialogue_manager_->show_dialogue(DialogueManager::DialogueState::INTRO);
}

// Animation setup methods have been moved to the Character classes

void MainScene::setup_collisions()
{
    // Create and initialize collision manager
    collision_manager_ = std::make_unique<CollisionManager>();
    collision_manager_->init(scene_state_);
    
    // Initialize boundary nodes
    collision_manager_->init_boundary(bottom_boundary_, scene_state_);
    collision_manager_->init_boundary(top_boundary_, scene_state_);
    collision_manager_->init_boundary(left_boundary_, scene_state_);
    collision_manager_->init_boundary(right_boundary_, scene_state_);
    collision_manager_->init_boundary(left_pillar_, scene_state_);
    collision_manager_->init_boundary(right_pillar_, scene_state_);

    // Configure world boundaries
    collision_manager_->add_boundary_collider(bottom_boundary_, Vector2(-17.3, 8.5), Vector2(17.75, 9.88));
    collision_manager_->add_boundary_collider(top_boundary_, Vector2(-17.3, -10.4), Vector2(17.75, -10.2));
    collision_manager_->add_boundary_collider(left_boundary_, Vector2(-18.3, -9.65), Vector2(-17.5, 9.88));
    collision_manager_->add_boundary_collider(right_boundary_, Vector2(17.6, -9.65), Vector2(17.75, 9.88));
    collision_manager_->add_boundary_collider(left_pillar_, Vector2(-12.9, -5.5), Vector2(-8.4, -1.3));
    collision_manager_->add_boundary_collider(right_pillar_, Vector2(8.4, -5.5), Vector2(13.0, -1.3));
    
    // Character objects
    auto& camera = root_.get_child<0>();
    auto& blue_witch_transform = camera.get_child<1>();
    auto& witch_transform = camera.get_child<2>();

    // Add AABB collision component to witch (player) and blue witch (NPC)
    collision_manager_->add_entity_collider(witch_transform, Vector2(-0.5f, -1.0f), Vector2(0.5f, 1.0f));
    collision_manager_->add_entity_collider(blue_witch_transform, Vector2(-0.5f, -1.0f), Vector2(0.5f, 1.0f));
    
    // Register boundary collision response
    collision_manager_->register_boundary_response([this](TransformNode* entity, TransformNode* boundary) 
        {
        handle_boundary_collision(entity, boundary);
        });

    // Register entity collision response for witches collision
    collision_manager_->register_entity_response([this, &blue_witch_transform, &witch_transform](TransformNode* entity_a, TransformNode* entity_b) 
        {
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

                    // Show appropriate dialogue based on find count
                    if (dialogue_manager_) {
                        if (find_count_ == 0) {
                            // First find
                            dialogue_manager_->show_dialogue(DialogueManager::DialogueState::FIRST_FIND);
                        } else if (find_count_ == 1) {
                            // Second find
                            dialogue_manager_->show_dialogue(DialogueManager::DialogueState::SECOND_FIND);
                        } else if (find_count_ == 2) {
                            // Third find - NPC should remain visible after this
                            dialogue_manager_->show_dialogue(DialogueManager::DialogueState::THIRD_FIND);
                        }
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

void MainScene::setup_audio() 
{
    // Create and initialize audio manager
    audio_manager_ = std::make_unique<AudioManager>();
    audio_manager_->init(scene_state_);
    
    // Initialize character audio
    player_->init_audio();
    blue_witch_->init_audio();
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
    // Update the dialogue manager
    if (dialogue_manager_) dialogue_manager_->update();
}

// Handle dialogue completion callback
void MainScene::handle_dialogue_completed(DialogueManager::DialogueState state)
{
    auto& camera = root_.get_child<0>();
    auto& blue_witch_transform = camera.get_child<1>();

    switch (state) {
        case DialogueManager::DialogueState::INTRO:
            // Intro dialogue completed
            dialogue_completed_ = true;
            blue_witch_transform.set_path_controlled(blue_witch_path_);
            break;
            
        case DialogueManager::DialogueState::FIRST_FIND:
            // First find dialogue completed
            waiting_for_dialogue_ = false;
            hide_npc();
            teleport_npc_to_location(14.6641f, 6.5282f);
            break;
            
        case DialogueManager::DialogueState::SECOND_FIND:
            // Second find dialogue completed
            waiting_for_dialogue_ = false;
            hide_npc();
            teleport_npc_to_location(3.3f, -2.525f);
            break;
            
        case DialogueManager::DialogueState::THIRD_FIND:
            // Third find dialogue completed
            waiting_for_dialogue_ = false;
            show_npc();
            
            // If the game is completed, show the game over menu
            if (game_completed_) SceneManager::get_instance()->push_scene_by_key("game_over_menu");
            break;
            
        default:
            break;
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
        if (find_count_ == 1) teleport_npc_to_location(14.6641f, 6.5282f);
        else if (find_count_ == 2) teleport_npc_to_location(3.3f, -2.525f);
        else teleport_npc_to_location(-15.55f, -7.625f);
    }
    
    // Special case for third find - NPC should remain visible
    if (find_count_ == 3 && waiting_for_dialogue_ == false && !blue_witch_transform.is_moving()) 
    {
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
    if (dialogue_manager_) 
    {
        if (find_number == 1) dialogue_manager_->show_dialogue(DialogueManager::DialogueState::FIRST_FIND);
        else if (find_number == 2) dialogue_manager_->show_dialogue(DialogueManager::DialogueState::SECOND_FIND);
        else if (find_number == 3) dialogue_manager_->show_dialogue(DialogueManager::DialogueState::THIRD_FIND);    
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
    // Process all collisions using the collision manager
    if (collision_manager_) collision_manager_->process_collisions();
}

void MainScene::handle_boundary_collision(TransformNode *entity, TransformNode *boundary)
{
    // Basic boundary collision. Just moves the player back to their previous position.
    entity->set_position(entity->get_prev_position_x(), entity->get_prev_position_y());
}

// Register a collision component with the collision system. 
void MainScene::register_collision_component(std::shared_ptr<CollisionComponent> component)
{
    if (collision_manager_) collision_manager_->add_component(component, CollisionSystem::CollisionType::ENTITY);
}

void MainScene::handle_audio()
{    
    auto &camera = root_.get_child<0>();
    auto &blue_witch_transform = camera.get_child<1>();
    auto &witch_transform = camera.get_child<2>();
    
    // Update 3D audio positioning using audio manager
    if (audio_manager_) {
        audio_manager_->update_audio_positions(&witch_transform, &blue_witch_transform);
    }
    
    // Process audio-related input actions
    player_->process_audio_actions(io_handler_, blue_witch_);
}

// Audio positioning is now handled by the AudioManager

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
    if (blue_witch_hidden_) hide_npc();
    else show_npc();

    // Update dialogue state based on loaded state
    if (waiting_for_dialogue_ && find_count_ > 0 && find_count_ <= 3) 
    {
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
