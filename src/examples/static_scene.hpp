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

namespace cge
{

// Define animated sprite with proper hierarchy
using AnimatedSprite = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;

// Parent camera with two child animated sprites
using AnimatedScene = CameraNodeT<AnimatedSprite, AnimatedSprite > ;

class StaticScene
{
  public:
    void init(SDLInfo *sdl_info, IoHandler *io_handler);
    void destroy();
    void render();
    void update(double delta);

  private:
    SDLInfo                 *sdl_info_;
    RootNodeT<AnimatedScene> root_;
    SceneState               scene_state_;
    IoHandler               *io_handler_;

    // Texture nodes are owned by StaticScene
    TextureNode golem_walk_texture_;
    TextureNode golem_idle_texture_;
    TextureNode witch_run_texture_;

    // Paths for NPCs
    Path witch_path_;

    // Helper methods for setup
    void setup_golem_animations();
    void setup_witch_animations();
};

} // namespace cge

#endif // EXAMPLES_STATIC_SCENE_HPP
