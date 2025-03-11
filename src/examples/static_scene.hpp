/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef EXAMPLES_STATIC_SCENE_HPP
#define EXAMPLES_STATIC_SCENE_HPP

#include "graph/analytics_node.hpp"
#include "graph/camera_node.hpp"
#include "graph/geometry_node.hpp"
#include "graph/node.hpp"
#include "graph/root_node.hpp"
#include "graph/scene_state.hpp"
#include "graph/sprite_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/transform_node.hpp"

#include "platform/io_handler.hpp"
#include "platform/path.hpp"

#include "platform/collision_system.hpp"

namespace cge
{

// Define animated sprite
using AnimatedSprite = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
using GameMap = TransformNodeT<TextureNodeT<GeometryNodeT<>>>;

// Parent camera with two child animated sprites and a zone transform node
using AnimatedScene = CameraNodeT<GameMap, AnimatedSprite, AnimatedSprite, TransformNodeT<>>;

class StaticScene
{
  public:
    void init(SDLInfo *sdl_info, IoHandler *io_handler);
    void destroy();
    void render();
    void update(double delta);

    // Collision management methods
    void             register_collision_component(CollisionComponent *component);

  private:
    SDLInfo                 *sdl_info_;
    RootNodeT<AnimatedScene> root_;
    SceneState               scene_state_;
    IoHandler               *io_handler_;

    // Texture nodes for specific sprite sheets
    TextureNode golem_walk_texture_;
    TextureNode golem_idle_texture_;
    TextureNode witch_run_texture_;

    // Paths for NPCs
    Path witch_path_;

    // Collision system
    CollisionSystem collision_system_;

    // Counter for witch passes
    int witch_zone_passes_{0};

    // Status of witch in zone
    bool witch_in_zone_{false};

    // Helper methods for setup
    void setup_golem_animations();
    void setup_witch_animations();
    void setup_collisions();
    void setup_trigger_zones();

    // Collision handling methods
    void handle_collisions();
    void handle_boundary_collision(TransformNode *entity, TransformNode *boundary);
    void handle_trigger_zone_collision(TransformNode *witch, TransformNode *zone);
};

} // namespace cge

#endif // EXAMPLES_STATIC_SCENE_HPP
