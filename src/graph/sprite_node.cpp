#include "graph/geometry_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/sprite_node.hpp"
#include "graph/scene_state.hpp"

namespace cge
{

void SpriteNode::init(SceneState& scene_state) 
{
    // Recursively initialize texture node and geometry node
    this->texture_node.init(scene_state);	
}

void SpriteNode::destroy() 
{ 

    this->texture_node.destroy();
}

void SpriteNode::draw(SceneState& scene_state)
{
    this->texture_node.draw(scene_state);
}

void SpriteNode::set_filepath(const std::string& path) { this->texture_node.set_filepath(path); }

void SpriteNode::set_transform(float x, float y, float angle)
{
    this->x = x;
    this->y = y;
    this->angle = angle;
}

// Texture node wrapper methods
void SpriteNode::set_color_mods(const uint8_t mods[3]) { this->texture_node.set_color_mods(mods); }
void SpriteNode::set_blend(bool blend) { this->texture_node.set_blend(blend); }
void SpriteNode::set_blend_alpha(uint8_t alpha) { this->texture_node.set_blend_alpha(alpha); }

// Geometry node wrapper methods
void SpriteNode::set_top_left(float x, float y) 
{ 
    auto& geometry_node = this->texture_node.get_child<0>();
    geometry_node.set_top_left(x, y);

}

void SpriteNode::set_top_right(float x, float y) 
{
    auto &geometry_node = this->texture_node.get_child<0>();
    geometry_node.set_top_right(x, y); 
}
void SpriteNode::set_bottom_left(float x, float y) 
{ 
    auto &geometry_node = this->texture_node.get_child<0>();
    geometry_node.set_bottom_left(x, y); 
} 

} // namespace cge

