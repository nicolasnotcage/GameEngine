/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/main_scene.hpp"
#include "platform/animation.hpp"
#include "platform/audio_engine.hpp"
#include "platform/collision_component.hpp" 
#include "platform/collision_system.hpp"

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

    SDL_SetRenderDrawColor(sdl_info->renderer, 28, 40, 51, 0);
    SDL_SetRenderDrawBlendMode(sdl_info->renderer, SDL_BLENDMODE_BLEND);

    // Reset scene state
    scene_state_.reset();
    scene_state_.sdl_info = sdl_info_;
    scene_state_.io_handler = io_handler_;

    // ------------------------------------
    //    Initialize Sprite Textures
    // ------------------------------------

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

    // Witch idle texture
    witch_idle_texture_.set_filepath("images/witch_idle.png");
    witch_idle_texture_.set_blend(true);
    witch_idle_texture_.set_blend_alpha(200);
    witch_idle_texture_.define_grid(1, 6, 64, 64);
    witch_idle_texture_.init(scene_state_);

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
    auto &golem_transform = camera.get_child<1>();
    auto &witch_transform = camera.get_child<2>();

    // Position golem
    golem_transform.right_translate(-15.55f, -7.625);
    golem_transform.right_scale(3.0f, 3.0f);

    // Add audio to golem
    auto *golem_audio = golem_transform.add_audio_component();
    golem_audio->set_sound("npc_clap");

    // Set witch transform if no save file exists
    if (!SaveManager::get_instance().save_exists()) witch_transform.right_translate(1.0f, 0.0f);

    // Scale witch
    witch_transform.right_scale(3.0f, 3.0f);

    // Add audio to witch
    auto *witch_audio = witch_transform.add_audio_component();
    witch_audio->set_sound("player_clap");

    // Set camera to follow player (witch)
    camera.set_target(&witch_transform, true);
    camera.set_follow_smoothness(1.0f);

    // Setup animations
    setup_golem_animations();
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

void MainScene::setup_golem_animations()
{
    auto &camera = root_.get_child<0>();
    auto &golem_transform = camera.get_child<1>();
    auto &golem_sprite = golem_transform.get_child<0>();

    // Create walk animation
    Animation walk_animation("run", true);
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

    // Associate the golem's transform with its sprite
    golem_transform.set_associated_sprite(&golem_sprite);
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
    witch_sprite.add_animation_with_texture(run_animation, &witch_run_texture_);
    witch_sprite.add_animation_with_texture(idle_animation, &witch_idle_texture_);

    // Set initial texture
    witch_sprite.set_texture(&witch_idle_texture_);

    // Start playing run animation by default
    witch_sprite.play("idle");

    // Set witch as player controlled and associate its transform with its sprite
    witch_transform.set_player_controlled();
    witch_transform.set_associated_sprite(&witch_sprite);
}

void MainScene::setup_collisions()
{
    auto &camera = root_.get_child<0>();
    auto &game_map_transform = camera.get_child<0>();
    auto &golem_transform = camera.get_child<1>();
    auto &witch_transform = camera.get_child<2>();

    // Configure world boundaries
    // 1. Bottom boundary (water on the bottom of the map)
    AABBCollisionComponent *boundary =
        game_map_transform.add_aabb_collider(Vector2(-17.725, 8.5), Vector2(17.75, 9.88));

    // TODO: Better map boundary handling
    // 2. Left boundary
    // 3. Right boundary
    // 4. Top boundary

    // Add AABB collision component to witch (player)
    AABBCollisionComponent *witch_collider =
        witch_transform.add_aabb_collider(Vector2(-0.5f, -1.0f), Vector2(0.5f, 1.0f));

    // Add AABB collision component to golem (NPC)
    AABBCollisionComponent *golem_collider =
        golem_transform.add_aabb_collider(Vector2(-0.5f, -1.0f), Vector2(0.5f, 1.0f));

    // Register components with the collision system
    collision_system_.add_component(boundary);
    collision_system_.add_component(golem_collider);
    collision_system_.add_component(witch_collider);
}

void MainScene::setup_trigger_zones()
{
    // Configure trigger zones here
}

void MainScene::setup_audio() 
{ 
    cge::AudioEngine *audio_engine = cge::AudioEngine::get_instance(); 
    if (!audio_engine->init(32, true))
    {
        std::cerr << "Failed to initialize AudioEngine in MainScene...\n";
        return;
    }

    // Locate files
    auto player_file_info = locate_path_for_filename("audio/player_clap.wav");
    auto npc_file_info = locate_path_for_filename("audio/npc_clap.wav");
    auto collision_sound_info = locate_path_for_filename("audio/creepy_ha_oneshot.wav");
    auto theme_sound_info = locate_path_for_filename("audio/theme_music.mp3");

    // Load sounds
    audio_engine->load_sound(player_file_info.path, "player_clap", false, false);
    audio_engine->load_sound(npc_file_info.path, "npc_clap", false, false);
    audio_engine->load_sound(collision_sound_info.path, "creepy_ha", false, false);
    audio_engine->load_sound(theme_sound_info.path, "theme_music", false, true);


    // Reserve channels for each sound
    audio_engine->reserve_channel_for_sound("creepy_ha", 0);
    audio_engine->reserve_channel_for_sound("npc_clap", 1);
    audio_engine->reserve_channel_for_sound("player_clap", 2);
    audio_engine->reserve_channel_for_sound("theme_music", 3);
}

void MainScene::destroy()
{
    root_.destroy();
    golem_walk_texture_.destroy();
    golem_idle_texture_.destroy();
    witch_run_texture_.destroy();
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

    // Handle boolean for laughing sound
    if (has_laughed_)
    { 
        time_to_laugh_ += delta;
        if (time_to_laugh_ >= 10)
        { 
            has_laughed_ = false;
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
    // Get collision pairs from the system
    auto collision_pairs = collision_system_.check_collisions();

    // Get nodes related to collision behavior
    auto &camera = root_.get_child<0>();
    auto &game_map_transform = camera.get_child<0>();
    auto &golem_transform = camera.get_child<1>();
    auto &witch_transform = camera.get_child<2>();

    // Track if witch and golem are colliding this frame
    bool witch_golem_colliding = false;

    // Handle each collision pair
    for(const auto &pair : collision_pairs)
    {
        // Get transform nodes
        TransformNode *transform_a = pair.first->get_owner();
        TransformNode *transform_b = pair.second->get_owner();

        // Handle boundary collisions
        if(transform_a == &game_map_transform)
        {
            handle_boundary_collision(transform_b, transform_a);
        }
        else if(transform_b == &game_map_transform)
        {
            handle_boundary_collision(transform_a, transform_b);
        }

        // Check for witch-golem collision
        if((transform_a == &witch_transform && transform_b == &golem_transform) ||
           (transform_b == &witch_transform && transform_a == &golem_transform))
        {
            witch_golem_colliding = true;
        }

        // TODO: Handle other collision types
    }

    // Handle laugh sound logic on player-npc collision
    if(witch_golem_colliding && !has_laughed_)
    {
        // Play sound only if not already in cooldown period
        AudioEngine::get_instance()->play_sound("creepy_ha", 1.0f);
        has_laughed_ = true;
        time_to_laugh_ = 0.0f;
    }

}

void MainScene::handle_boundary_collision(TransformNode *entity, TransformNode *boundary)
{
    // Basic boundary collision. Just moves the player back to their previous position.
    entity->set_position(entity->get_prev_position_x(), entity->get_prev_position_y());

    std::cout << "AABB-AABB COLLISION: Player collided with world boundary and had position reset.\n";
}

// Register a collision component with the collision system. 
void MainScene::register_collision_component(CollisionComponent *component)
{
    collision_system_.add_component(component);
}

 void MainScene::handle_audio()
{    
    // Increment timer
    npc_audio_timer_ += scene_state_.delta;

    // Play audio if timer satisfied
    if (npc_audio_timer_ >= time_to_clap_) 
    { 
        auto &camera = root_.get_child<0>();
        auto &golem_transform = camera.get_child<1>();
        if (auto* golem_audio = golem_transform.get_audio_component())
        { 
            golem_audio->play(1.0f);
        }

        // Reset timer
        npc_audio_timer_ = 0.0f;
    }

    // Check for spacebar to play player clap
    const GameActionList &actions = io_handler_->get_game_actions();
    for(uint8_t i = 0; i < actions.num_actions; i++)
    {
        if(actions.actions[i] == GameAction::PLAYER_CLAP)
        {
            auto &camera = root_.get_child<0>();
            auto &witch_transform = camera.get_child<2>();
            if(auto *witch_audio = witch_transform.get_audio_component())
            {
                witch_audio->play(1.0f);
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

    std::cout << "Serialized Test Values\nTest Float: " << test_float_
              << "\nTest Int: " << test_int_ 
              << "\nTest Bool: " << test_bool_
              << "\nTest String: " << test_string_ << "\n";
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

    std::cout << "Deserialized Test Values\nTest Float: " << test_float_
              << "\nTest Int: " << test_int_ 
              << "\nTest Bool: " << test_bool_
              << "\nTest String: " << test_string_ << "\n";
}

} // namespace cge
