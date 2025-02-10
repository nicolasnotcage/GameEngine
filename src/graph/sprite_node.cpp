#include "graph/geometry_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/sprite_node.hpp"
#include "graph/scene_state.hpp"
#include <iostream>

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

void SpriteNode::set_filepath(const std::string& path)
{
    if (auto tex_node = this->get_child_by_type<TextureNode>()) 
    { 
        tex_node->set_filepath(path); 
    }
    else 
    { 
        std::cerr << "Warning: No TextureNode found in SpriteNode.\n"; 
    }    
}

void SpriteNode::set_blend(bool blend)
{
    if(auto tex_node = this->get_texture_node()) { tex_node->set_blend(blend); }
}

void SpriteNode::set_blend_alpha(uint8_t alpha) 
{
    if(auto tex_node = this->get_texture_node()) { tex_node->set_blend_alpha(alpha); }
}

void SpriteNode::set_color_mods(const uint8_t mods[3]) 
{
    if(auto tex_node = this->get_texture_node()) { tex_node->set_color_mods(mods); }
}

void SpriteNode::set_top_left(float x, float y) 
{
    if(auto geo_node = this->get_geometry_node()) { geo_node->set_top_left(x, y); }
}

void SpriteNode::set_top_right(float x, float y) 
{
    if(auto geo_node = this->get_geometry_node()) { geo_node->set_top_right(x, y); }
}

void SpriteNode::set_bottom_left(float x, float y) 
{
    if(auto geo_node = this->get_geometry_node()) { geo_node->set_bottom_left(x, y); }
}

// Returns pointer to texture node child
TextureNode *SpriteNode::get_texture_node() 
{ 
    if (auto tex_node = this->get_child_by_type<TextureNode>()) { return tex_node; }
    std::cerr << "Warning: No TextureNode found in SpriteNode.\n";
    return nullptr; 
}

GeometryNode* SpriteNode::get_geometry_node() 
{
    if (auto tex_node = this->get_texture_node())
    {
        if (auto geo_node = tex_node->get_child_by_type<GeometryNode>())
        { 
            return geo_node;
        }
        std::cerr << "Warning: No GeometryNode found in TextureNode.\n";
    }

    return nullptr;
}
} // namespace cge

