/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef EXAMPLES_MAIN_SCENE_HPP
#define EXAMPLES_MAIN_SCENE_HPP

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
using PlayerSprite = TransformNodeT<SpriteNodeT<GeometryNodeT<>>, GameText>;

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

    // Texture nodes for specific sprite sheets
    TextureNode blue_witch_run_texture_;
    TextureNode blue_witch_idle_texture_;
    TextureNode white_witch_run_texture_;
    TextureNode white_witch_idle_texture_;
    TextureNode blue_witch_transparent_texture_;

    // Transform nodes for map boundaries and obstacles
    TransformNode bottom_boundary_;
    TransformNode top_boundary_;
    TransformNode left_boundary_;
    TransformNode right_boundary_;
    TransformNode left_pillar_;
    TransformNode right_pillar_;

    // Texture nodes for game text
    TextureNode intro_1_;
    TextureNode intro_2_;

    // Collision system
    CollisionSystem collision_system_;

    // Helper methods for scene setup
    void setup_npc_animations();
    void setup_witch_animations();
    void setup_collisions();
    void setup_trigger_zones();
    void setup_audio();

    // Collision and audio handling methods
    void handle_collisions();
    void handle_boundary_collision(TransformNode *entity, TransformNode *boundary);
    void handle_audio();

    // Gameplay specific data
    float time_to_clap_{1.0f};  // Delay in seconds before clapping
    float npc_audio_timer_{0.0f};
    float time_to_laugh_{0.0f};
    bool  has_laughed_{false};
    bool  waiting_to_clap_{false};  // Flag to track if we're waiting to clap
    int   laugh_channel_id{-1};
    Path blue_witch_path_;
    bool dialogue_completed_{ false };
    bool blue_witch_hidden_{ false };

    // Serializer tests
    float test_float_{0.0};
    int test_int_{0};
    bool test_bool_{true};
    std::string test_string_{"test"};
};

} // namespace cge

#endif // EXAMPLES_MAIN_SCENE_HPP
