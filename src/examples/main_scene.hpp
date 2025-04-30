/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef EXAMPLES_MAIN_SCENE_HPP
#define EXAMPLES_MAIN_SCENE_HPP

#include "examples/player.hpp"
#include "examples/npc.hpp"
#include "examples/dialogue_manager.hpp"
#include "platform/collision_manager.hpp"
#include "platform/audio_manager.hpp"

#include "graph/camera_node.hpp"
#include "graph/geometry_node.hpp"
#include "graph/node.hpp"
#include "graph/root_node.hpp"
#include "graph/scene_state.hpp"
#include "graph/sprite_node.hpp"
#include "graph/text_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/transform_node.hpp"

#include "platform/audio_component.hpp"
#include "platform/collision_system.hpp"
#include "platform/io_handler.hpp"
#include "platform/path.hpp"
#include "platform/scene.hpp"

#include <string>
#include <memory>

namespace cge
{

// Define animated sprite
using AnimatedSprite = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
using GameMap = TransformNodeT<TextureNodeT<GeometryNodeT<>>>;
using GameText = TransformNodeT<TextNodeT<GeometryNodeT<>>>;
using PlayerSprite = TransformNodeT<SpriteNodeT<GeometryNodeT<>>, GameText, GameText, GameText, GameText>;

// Parent camera with two child animated sprites and a zone transform node
using AnimatedScene = CameraNodeT<GameMap, 
                                  AnimatedSprite, 
                                  PlayerSprite>;

class MainScene : public Scene
{
  public:

    // Initialize the scene with sdl_info and io_handler
    void init(SDLInfo *sdl_info, IoHandler *io_handler);

    // Destroy the scene
    void destroy();

    // Render the scene
    void render();

    // Update the scene with delta time.
    void update(double delta);

    // Register collision components with the stored collision system. 
    void register_collision_component(std::shared_ptr<CollisionComponent> component);

    // Serialize the scene.
    void serialize(Serializer& serializer) const override;

    // Deserialize the scene.
    void deserialize(Serializer& serializer) override;

  private:
    SDLInfo                 *sdl_info_;
    RootNodeT<AnimatedScene> root_;
    SceneState               scene_state_;
    IoHandler               *io_handler_;

    // Manager and character pointers
    std::shared_ptr<Player> player_;
    std::shared_ptr<NPC> blue_witch_;
    std::unique_ptr<DialogueManager> dialogue_manager_;
    std::unique_ptr<CollisionManager> collision_manager_;
    std::unique_ptr<AudioManager> audio_manager_;

    // Transform nodes for map boundaries and obstacles
    TransformNode bottom_boundary_;
    TransformNode top_boundary_;
    TransformNode left_boundary_;
    TransformNode right_boundary_;
    TransformNode left_pillar_;
    TransformNode right_pillar_;

    // --------------------
    //    Helper Methods
    // --------------------
    // Setup the scene's collision manager.
    void setup_collisions();

    // Setup the scene's audio configuration
    void setup_audio();

    // Configure the scene's camera
    void setup_camera();

    // Configure the scene's game map
    void setup_game_map();

    // Configure the scene's characters
    void setup_characters();

    // Configure the text nodes for scene dialogue
    void configure_dialogue_text_node(TransformNode& transform_node, TextNode& text_node, TextureNode* texture);

    // Configure the scene's dialogue nodes
    void setup_dialogue_nodes();

    // Handle dialogue state and state transitions
    void handle_dialogue_state();

    // Handle NPC state and state transitions
    void handle_npc_state();

    // Update scene based on dialogue state
    void handle_dialogue_completed(DialogueManager::DialogueState state);

    // Show dialogue based on player's in-game progress.
    // find_number is the number of times that the NPC has been 
    // found by the player in the current gameplay session.
    void show_dialogue_for_find(int find_number);

    // Handle boundary collisions for entity and boundary nodes.
    void handle_boundary_collision(TransformNode *entity, TransformNode *boundary);

    // Handle audio-related game behavior.
    void handle_audio();

    // --------------------------
    //     Game State Data
    // --------------------------
    float time_to_clap_{1.0f};                  // Delay in seconds before clapping
    Path blue_witch_path_;                      // The path that the NPC will follow after the tutorial
    bool intro_dialogue_completed_{ false };    // Whether the intro dialogue has been completed
    int find_count_{0};                         // 0 = not found yet, 1 = found once, 2 = found twice, 3 = found three times
    bool waiting_for_dialogue_{false};          // Flag to track if we're waiting for dialogue to complete
    bool game_completed_{false};                // Flag to track if the game is completed (found NPC three times)
};

} // namespace cge

#endif // EXAMPLES_MAIN_SCENE_HPP
