#include "graph/geometry_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/sprite_node.hpp"
#include "graph/scene_state.hpp"

namespace cge
{
void SpriteNode::init(SceneState& scene_state) 
{
	this->texture_node->init(scene_state);
    this->geometry_node->init(scene_state);
}

void SpriteNode::destroy() 
{ 
    this->texture_node->destroy();
    this->geometry_node->destroy();
}

void SpriteNode::draw(SceneState& scene_state)
{
    this->texture_node->draw(scene_state);
    this->geometry_node->draw(scene_state);
}

void SpriteNode::set_filepath(const std::string& path) { this->texture_node->set_filepath(path); }

void SpriteNode::set_transform(float x, float y, float angle)
{
    this->x = x;
    this->y = y;
    this->angle = angle;
}

} // namespace cge

