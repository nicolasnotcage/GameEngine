#include "graph/sprite_node.hpp"
#include "graph/geometry_node.hpp"
#include "graph/scene_state.hpp"
#include "graph/texture_node.hpp"

namespace cge
{

void SpriteNode::init(SceneState &scene_state) { init_children(scene_state); }
void SpriteNode::draw(SceneState &scene_state) { draw_children(scene_state); }

void SpriteNode::destroy()
{
    destroy_children();
    clear_children();
}

void SpriteNode::update(SceneState &scene_state)
{ 
    update_children(scene_state); 
}

} // namespace cge
