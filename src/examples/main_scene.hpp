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
    void init(SDLInfo *sdl_info, IoHandler *io_handler);
    void destroy();
    void render();
    void update(double delta);

    // Register collision components with the stored collision system. 
    void register_collision_component(std::shared_ptr<CollisionComponent> component);

    // Serializable overrides
    void serialize(Serializer& serializer) const override;
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

    // No longer needed as we're using CollisionManager
    // CollisionSystem collision_system_;

    // Helper methods for scene setup
    void setup_collisions();
    void setup_audio();

    // Helper method for configuring dialogue text nodes

    // Scene graph configuration helpers
    void setup_camera();
    void setup_game_map();
    void setup_characters();
    void configure_dialogue_text_node(TransformNode& transform_node, TextNode& text_node, TextureNode* texture);
    void setup_dialogue_nodes();

    // Game logic helpers
    void handle_dialogue_state();
    void handle_npc_state();
    void handle_dialogue_completed(DialogueManager::DialogueState state);
    void show_dialogue_for_find(int find_number);

    // Collision and audio handling methods
    void handle_boundary_collision(TransformNode *entity, TransformNode *boundary);
    void handle_audio();

    // Gameplay specific data
    float time_to_clap_{1.0f};  // Delay in seconds before clapping
    float npc_audio_timer_{0.0f};
    bool  waiting_to_clap_{false};  // Flag to track if we're waiting to clap
    int   laugh_channel_id{-1};
    Path blue_witch_path_;
    bool dialogue_completed_{ false };
    int find_count_{0}; // 0 = not found yet, 1 = found once, 2 = found twice, 3 = found three times
    bool waiting_for_dialogue_{false}; // Flag to track if we're waiting for dialogue to complete
    bool game_completed_{false}; // Flag to track if the game is completed (found NPC three times)
};

} // namespace cge

#endif // EXAMPLES_MAIN_SCENE_HPP
