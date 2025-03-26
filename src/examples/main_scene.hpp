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
#include "graph/texture_node.hpp"
#include "graph/transform_node.hpp"

#include "platform/audio_component.hpp"
#include "platform/collision_system.hpp"
#include "platform/io_handler.hpp"
#include "platform/path.hpp"

namespace cge
{

// Define animated sprite
using AnimatedSprite = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
using GameMap = TransformNodeT<TextureNodeT<GeometryNodeT<>>>;

// Parent camera with two child animated sprites and a zone transform node
using AnimatedScene = CameraNodeT<GameMap, AnimatedSprite, AnimatedSprite, TransformNodeT<>>;

class MainScene
{
  public:
    void init(SDLInfo *sdl_info, IoHandler *io_handler);
    void destroy();
    void render();
    void update(double delta);

    // Register collision components with the stored collision system. 
    void register_collision_component(CollisionComponent *component);

  private:
    SDLInfo                 *sdl_info_;
    RootNodeT<AnimatedScene> root_;
    SceneState               scene_state_;
    IoHandler               *io_handler_;

    // Texture nodes for specific sprite sheets
    TextureNode golem_walk_texture_;
    TextureNode golem_idle_texture_;
    TextureNode witch_run_texture_;
    TextureNode witch_idle_texture_;

    // Collision system
    CollisionSystem collision_system_;

    // Helper methods for scene setup
    void setup_golem_animations();
    void setup_witch_animations();
    void setup_collisions();
    void setup_trigger_zones();
    void setup_audio();

    // Collision and audio handling methods
    void handle_collisions();
    void handle_boundary_collision(TransformNode *entity, TransformNode *boundary);
    void handle_audio();

    // Gameplay specific data
    float time_to_clap_{3.0f};
    float npc_audio_timer_{0.0f};
    float time_to_laugh_{0.0f};
    bool  has_laughed_{false};
    int   laugh_channel_id{-1};
};

} // namespace cge

#endif // EXAMPLES_MAIN_SCENE_HPP
