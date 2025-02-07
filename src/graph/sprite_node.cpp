#include "graph/geometry_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/sprite_node.hpp"
#include "graph/scene_state.hpp"

namespace cge
{

void SpriteNode::init(SceneState &scene_state) { init_children(scene_state);  }
void SpriteNode::draw(SceneState &scene_state) { draw_children(scene_state); }
void SpriteNode::update(SceneState& scene_state)
{
    // TODO: Do something.
}

void SpriteNode::destroy() 
{ 
    destroy_children();
    clear_children();
}

} // namespace cge

