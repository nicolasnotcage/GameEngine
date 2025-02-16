/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GRAPH_SCENE_STATE_HPP
#define GRAPH_SCENE_STATE_HPP

#include "platform/types.hpp"
#include "platform/math.hpp"

namespace cge
{

// Forward declaration
class TextureNode;

// Struct containing pointers to an SDLInfo struct and a texture node. Contains 
// a reset function to set both data members to nullptr.
struct SceneState
{
    SDLInfo     *sdl_info;
    TextureNode *texture_node;
    float        delta;
    MatrixStack  matrix_stack;

    void reset();
};

} // namespace cge

#endif // GRAPH_SCENE_STATE_HPP
