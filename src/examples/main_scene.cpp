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

    // Setup scene components
    setup_camera();
    setup_game_map();
    setup_characters();
    setup_dialogue_nodes();

    // Setup systems
    setup_collisions();
    setup_audio();

    // Initialize characters
    player_->init(scene_state_);
    blue_witch_->init(scene_state_);

    // Initialize root node
    root_.init(scene_state_);
}

// Setup camera
void MainScene::setup_camera()
{
    auto &camera = root_.get_child<0>();
    camera.get_camera().set_dimensions(20.0f, 15.0f);
    camera.get_camera().set_position(0.0f, 0.0f);
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
    auto &player_transform = camera.get_child<2>();
    auto &player_sprite = player_transform.get_child<0>();

    // Create character objects
    blue_witch_ = std::make_shared<NPC>(&blue_witch_transform, &blue_witch_sprite);
    player_ = std::make_shared<Player>(&player_transform, &player_sprite);

    // Configure initial NPC path
    blue_witch_path_.add_point(4.0f, 1.0f, 0.5f);  // Start position
    blue_witch_path_.add_point(4.0f, 2.5f, 0.5f);  // Move down
    blue_witch_path_.set_looping(false);

    // Set camera to follow player
    camera.set_target(&player_transform, true);
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
    
    // Create dialogue manager; don't initialize until textures are configured
    dialogue_manager_ = std::make_unique<DialogueManager>();
    
    // Register dialogue textures by filepath
    dialogue_manager_->register_texture("intro_1", "images/game_text/intro_1.png");
    dialogue_manager_->register_texture("intro_2", "images/game_text/intro_2.png");
    dialogue_manager_->register_texture("intro_3", "images/game_text/intro_3.png");
    dialogue_manager_->register_texture("intro_4", "images/game_text/intro_4.png");
    dialogue_manager_->register_texture("intro_5", "images/game_text/intro_5.png");
    dialogue_manager_->register_texture("first_find", "images/game_text/first_find.png");
    dialogue_manager_->register_texture("second_find", "images/game_text/second_find.png");
    dialogue_manager_->register_texture("third_find", "images/game_text/third_find.png");
    
    // Configure and register dialogue nodes
    auto &intro_text_transform = camera.get_child<2>().get_child<1>();
    auto &intro_text_node = intro_text_transform.get_child<0>();
    configure_dialogue_text_node(intro_text_transform, intro_text_node, nullptr);
    dialogue_manager_->register_text_node("intro", &intro_text_node);

    // First find
    auto& first_find_transform = camera.get_child<2>().get_child<2>();
    auto& first_find_node = first_find_transform.get_child<0>();
    configure_dialogue_text_node(first_find_transform, first_find_node, nullptr);
    dialogue_manager_->register_text_node("first_find", &first_find_node);

    // Second find
    auto& second_find_transform = camera.get_child<2>().get_child<3>();
    auto& second_find_node = second_find_transform.get_child<0>();
    configure_dialogue_text_node(second_find_transform, second_find_node, nullptr);
    dialogue_manager_->register_text_node("second_find", &second_find_node);

    // Third find
    auto& third_find_transform = camera.get_child<2>().get_child<4>();
    auto& third_find_node = third_find_transform.get_child<0>();
    configure_dialogue_text_node(third_find_transform, third_find_node, nullptr);
    dialogue_manager_->register_text_node("third_find", &third_find_node);
    
    // Register dialogue completion callback
    dialogue_manager_->register_on_dialogue_completed_callback(
        [this](DialogueManager::DialogueState state) 
        {
            handle_dialogue_completed(state);
        });
    
    // Show intro dialogue if not completed
    if (!intro_dialogue_completed_) dialogue_manager_->show_dialogue(DialogueManager::DialogueState::INTRO);

    // Initialize textures now that nodes are configured
    dialogue_manager_->init(scene_state_);
}

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
        if (witches_colliding && blue_witch_->is_hidden())
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

                    // Show the NPC
                    blue_witch_->show();

                    // Play found sound
                    AudioEngine::get_instance()->play_sound("success", 1.0f);

                    // Get camera reference for text nodes
                    auto& camera = root_.get_child<0>();

                    // Show appropriate dialogue based on find count
                    if (dialogue_manager_) 
                    {
                        if (find_count_ == 0) dialogue_manager_->show_dialogue(DialogueManager::DialogueState::FIRST_FIND);
                        else if (find_count_ == 1) dialogue_manager_->show_dialogue(DialogueManager::DialogueState::SECOND_FIND);
                        else if (find_count_ == 2) dialogue_manager_->show_dialogue(DialogueManager::DialogueState::THIRD_FIND);
                    }

                    // Set waiting for dialogue flag
                    waiting_for_dialogue_ = true;

                    // Increment find count
                    find_count_++;
                    
                    // Check if the game is completed (found NPC three times)
                    if (find_count_ == 3) 
                    {
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
}

void MainScene::destroy()
{
    // Destroy character textures
    if (player_) player_->destroy_textures();
    if (blue_witch_) blue_witch_->destroy_textures();
    if (dialogue_manager_) dialogue_manager_->destroy_textures();
    
    // Destroy boundary nodes
    bottom_boundary_.destroy();
    top_boundary_.destroy();
    left_boundary_.destroy();
    right_boundary_.destroy();
    left_pillar_.destroy();
    right_pillar_.destroy();
    
    // Destroy root node
    root_.destroy();
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
    player_->handle_input_actions(io_handler_);

    // Handle general collisions using the collision system
    collision_manager_->process_collisions();
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
    switch (state) 
    {
        case DialogueManager::DialogueState::INTRO:
            // Intro dialogue completed
            intro_dialogue_completed_ = true;
            blue_witch_->set_path(blue_witch_path_);
            break;
            
        case DialogueManager::DialogueState::FIRST_FIND:
            // First find dialogue completed
            waiting_for_dialogue_ = false;
            blue_witch_->hide();
            blue_witch_->teleport_to(14.6641f, 6.5282f);
            break;
            
        case DialogueManager::DialogueState::SECOND_FIND:
            // Second find dialogue completed
            waiting_for_dialogue_ = false;
            blue_witch_->hide();
            blue_witch_->teleport_to(3.3f, -2.525f);
            break;
            
        case DialogueManager::DialogueState::THIRD_FIND:
            // Third find dialogue completed
            waiting_for_dialogue_ = false;
            blue_witch_->show();
            
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
    // Has the NPC traveled far enough to satisfy the visual effect?
    bool has_traveled_enough = blue_witch_->get_position_y() >= blue_witch_path_.get_point(1).y / 1.1f;

    // Hide witch and move to new position
    if (intro_dialogue_completed_         // Dialogue completed
        && has_traveled_enough      // Pathed far enough for effect
        && find_count_ == 0)        // Round has just started
    {
        blue_witch_->hide();
        blue_witch_->teleport_to(-15.55f, -7.625f);
    }
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

// Basic boundary collision. Just moves the player back to their previous position.
void MainScene::handle_boundary_collision(TransformNode *entity, TransformNode *boundary)
{
    entity->set_position(entity->get_prev_position_x(), entity->get_prev_position_y());
}

// Register a collision component with the collision system. 
void MainScene::register_collision_component(std::shared_ptr<CollisionComponent> component)
{
    if (collision_manager_) collision_manager_->add_component(component, CollisionSystem::CollisionType::ENTITY);
}

void MainScene::handle_audio()
{        
    // Update 3D audio positioning using audio manager
    if (audio_manager_) audio_manager_->update_audio_positions(player_, blue_witch_);
    
    // Process audio-related input actions
    player_->process_audio_actions(io_handler_, blue_witch_);
}

// Serialization overrides
void MainScene::serialize(Serializer& serializer) const
{
    // Delegate serialization to the Player, NPC, and DialogueManager classes
    if (player_) player_->serialize(serializer);
    if (blue_witch_) blue_witch_->serialize(serializer);
    if (dialogue_manager_) dialogue_manager_->serialize(serializer);
    
    // Serialize game state flags
    serializer.write("find_count", find_count_);
    serializer.write("waiting_for_dialogue", waiting_for_dialogue_);
    serializer.write("game_completed", game_completed_);
    serializer.write("dialogue_completed", intro_dialogue_completed_);
}

void MainScene::deserialize(Serializer& serializer)
{    
    // First check if the game was completed
    bool game_completed = false;
    serializer.read("game_completed", game_completed);
    
    // If the game was completed, don't load the saved state
    if (game_completed) 
    {
        std::cout << "Loading a completed game - starting a new game instead." << std::endl;
        return;
    }
    
    // Delegate deserialization to the Player, NPC, and DialogueManager classes
    if (player_) player_->deserialize(serializer);
    if (blue_witch_) blue_witch_->deserialize(serializer);
    if (dialogue_manager_) dialogue_manager_->deserialize(serializer);
    
    // Deserialize game state flags
    serializer.read("find_count", find_count_);
    serializer.read("waiting_for_dialogue", waiting_for_dialogue_);
    serializer.read("game_completed", game_completed_);
    serializer.read("dialogue_completed", intro_dialogue_completed_);
}

} // namespace cge
