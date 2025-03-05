/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "graph/geometry_node.hpp"
#include "graph/node.hpp"
#include "graph/root_node.hpp"
#include "graph/scene_state.hpp"
#include "graph/sprite_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/analytics_node.hpp"
#include "graph/transform_node.hpp"
#include "graph/camera_node.hpp"
#include "platform/io_handler.hpp"

namespace cge
{

using BasicSprite = TransformNodeT<TextureNodeT<GeometryNodeT<>>>;

// Parent sprite with two child sprites
using HierarchicalSprite =
    CameraNodeT<
        BasicSprite,
        BasicSprite>;

class StaticScene
{
  public:
    void init(SDLInfo *sdl_info, IoHandler *io_handler);

    void destroy();

    void render();

    void update(double delta);

  private:
    SDLInfo                      *sdl_info_;
    RootNodeT<HierarchicalSprite> root_;
    SceneState                    scene_state_;
    IoHandler                     *io_handler_;
};

} // namespace cge
