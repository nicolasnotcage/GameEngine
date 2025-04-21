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

    // Configure in-game text
    auto &text_transform = root_.get_child<0>().get_child<2>().get_child<1>();
    auto &text_node = text_transform.get_child<0>();
    text_transform.right_translate(0, 1.8f);
    text_transform.right_scale(3.0f, 1.0f);
    text_node.push_texture(&intro_1_);
    text_node.push_texture(&intro_2_);
    text_node.set_should_render(true);

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

    // Add audio to golem with 3D settings
    auto *blue_witch_audio = blue_witch_transform.add_audio_component();
    blue_witch_audio->set_sound("npc_clap");
    blue_witch_audio->set_min_distance(1.0f);
    blue_witch_audio->set_max_distance(10.0f);

    // Set witch transform if no save file exists
    if (!SaveManager::get_instance().save_exists()) witch_transform.right_translate(1.0f, 0.0f);

    // Scale witch
    witch_transform.right_scale(3.0f, 3.0f);

    // Add audio to witch
    auto *witch_audio = witch_transform.add_audio_component();
    witch_audio->set_sound("whistle");

    // Set camera to follow player (witch)
    camera.set_target(&witch_transform, true);
    camera.set_follow_smoothness(1.0f);

    // Setup animations
    setup_npc_animations();
    setup_witch_animations();

    // Setup systems
    setup_collisions();
    setup_trigger_zones();
    setup_audio();

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
            // Make blue witch visible again
            auto& blue_witch_sprite = blue_witch_transform.get_child<0>();
            
            // Re-enable automatic animation switching
            blue_witch_sprite.set_auto_animation_enabled(true);
            
            // Set the idle animation and texture
            blue_witch_sprite.set_texture(&blue_witch_idle_texture_);
            blue_witch_sprite.play("idle");
            
            // Play found sound
            AudioEngine::get_instance()->play_sound("creepy_ha", 1.0f);
            
            // Mark witch as found
            blue_witch_hidden_ = false;
            has_laughed_ = true;
            time_to_laugh_ = 0.0f;
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
    auto collision_sound_info = locate_path_for_filename("audio/creepy_ha_oneshot.wav");
    auto theme_sound_info = locate_path_for_filename("audio/theme_music.mp3");

    // Load sounds - note that npc_clap is now a 3D sound
    audio_engine->load_sound(player_file_info.path, "whistle", false, false);
    audio_engine->load_sound(npc_file_info.path, "npc_clap", true, false); // Set as 3D sound
    audio_engine->load_sound(collision_sound_info.path, "creepy_ha", false, false);
    audio_engine->load_sound(theme_sound_info.path, "theme_music", false, true);


    // Reserve channels for each sound
    audio_engine->reserve_channel_for_sound("creepy_ha", 0);
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

    // Check if dialogue is not rendered anymore (completed)
    if (!dialogue_completed_ && !text_node.is_rendered()) 
    {
        dialogue_completed_ = true;
        blue_witch_transform.set_path_controlled(blue_witch_path_);
    }

    // Check if path is completed and witch should be hidden
    if (dialogue_completed_ 
        && !blue_witch_hidden_ 
        && !blue_witch_transform.is_moving() 
        && blue_witch_transform.get_position_y() >= 1.5f) 
    {
        // Set new pathing position (Stationary)
        Path top_left_path{};
        top_left_path.add_point(-15.55f, -7.625, 0.0f);
        top_left_path.set_looping(false);

        // Hide witch and move to top left
        auto& blue_witch_sprite = blue_witch_transform.get_child<0>();
        
        // Disable automatic animation switching to prevent overriding the hidden animation
        blue_witch_sprite.set_auto_animation_enabled(false);
        
        // Set the hidden animation and texture
        blue_witch_sprite.set_texture(&blue_witch_transparent_texture_);
        blue_witch_sprite.play("hidden");
        
        // Move the witch to the top left
        blue_witch_transform.set_path_controlled(top_left_path);
        blue_witch_transform.set_position(-15.55f, -7.625);
        
        blue_witch_hidden_ = true;
    }

    // Update 3D audio listener position at the beginning of the frame
    Vector2 witch_position(witch_transform.get_position_x(), witch_transform.get_position_y());
    AudioEngine::get_instance()->set_3d_listener_position(witch_position);

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
    
    // Update witch's audio component position for 3D audio
    if (auto* blue_witch_audio = blue_witch_transform.get_audio_component())
    {
        // Force update of the witch's position in the audio component
        float blue_witch_x = blue_witch_transform.get_position_x();
        float blue_witch_y = blue_witch_transform.get_position_y();
        
        // Debug output for 3D audio positions
        float witch_x = witch_transform.get_position_x();
        float witch_y = witch_transform.get_position_y();
        float distance = std::sqrt((witch_x - blue_witch_x) * (witch_x - blue_witch_x) +
                                  (witch_y - blue_witch_y) * (witch_y - blue_witch_y));
        
        // Make sure 3D position is updated
        blue_witch_audio->update_position();
        
        // Configure 3D audio parameters - using smaller values for better effect
        blue_witch_audio->set_min_distance(1.0f);  // Closer min distance for more pronounced effect
        blue_witch_audio->set_max_distance(10.0f); // Shorter max distance for more noticeable falloff
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

            // Make blue witch clap in response if hidden
            if (auto* blue_witch_audio = blue_witch_transform.get_audio_component()) 
            {
                if (blue_witch_hidden_) blue_witch_audio->play(1.0f);
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
    
    // Update FMOD system once per tick; per FMOD Core documentation
    cge::AudioEngine::get_instance()->update();
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
