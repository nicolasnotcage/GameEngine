/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "graph/root_node.hpp"
#include "graph/scene_state.hpp"

namespace cge
{

class DynamicScene
{
  public:
    void init(SDLInfo *sdl_info);

    void destroy();

    void render();

  private:
    SDLInfo   *sdl_info_;
    RootNode   root_;
    SceneState scene_state_;
};

} // namespace cge
