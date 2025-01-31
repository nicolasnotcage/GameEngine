/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GRAPH_SCENE_STATE_HPP
#define GRAPH_SCENE_STATE_HPP

#include "platform/types.hpp"

namespace cge
{

// Forward declaration
class TextureNode;

struct SceneState
{
    SDLInfo     *sdl_info;
    TextureNode *texture_node;

    void reset();
};

} // namespace cge

#endif // GRAPH_SCENE_STATE_HPP
