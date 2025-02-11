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
    // Do something
}

TextureNode *SpriteNode::get_texture_node()
{
    if(auto *tex_node = get_child_by_type<TextureNode>()) { return tex_node; }
    std::cerr << "Warning: SpriteNode has no instance of TextureNode.\n";
    return nullptr;
}

// TODO: Put this function inside of TextureNode
GeometryNode *SpriteNode::get_geometry_node()
{
    if(auto *tex_node = get_texture_node())
    {
        if(auto *geo_node = tex_node->get_child_by_type<GeometryNode>()) { return geo_node; }
    }
    std::cerr << "Warning: TextureNode has no instance of GeometryNode.\n";
    return nullptr;
}

// Texture manipulation methods
void SpriteNode::set_filepath(const std::string &path)
{
    if(auto *tex_node = get_texture_node()) { tex_node->set_filepath(path); }
}

void SpriteNode::set_blend(bool blend)
{
    if(auto *tex_node = get_texture_node()) { tex_node->set_blend(blend); }
}

void SpriteNode::set_blend_alpha(uint8_t alpha)
{
    if(auto *tex_node = get_texture_node()) { tex_node->set_blend_alpha(alpha); }
}

void SpriteNode::set_color_mods(const uint8_t mods[3])
{
    if(auto *tex_node = get_texture_node()) { tex_node->set_color_mods(mods); }
}

// Geometry manipulation methods
void SpriteNode::set_top_left(float x, float y)
{
    if(auto *geo_node = get_geometry_node()) { geo_node->set_top_left(x, y); }
}

void SpriteNode::set_top_right(float x, float y)
{
    if(auto *geo_node = get_geometry_node()) { geo_node->set_top_right(x, y); }
}

void SpriteNode::set_bottom_left(float x, float y)
{
    if(auto *geo_node = get_geometry_node()) { geo_node->set_bottom_left(x, y); }
}

void SpriteNode::set_flip_horizontal(bool flip_h) 
{
    if(auto *tex_node = get_texture_node()) tex_node->set_flip_horizontal(flip_h);
}

void SpriteNode::set_flip_vertical(bool flip_v)
{
    if(auto *tex_node = get_texture_node()) tex_node->set_flip_vertical(flip_v);
}

void SpriteNode::set_source_rect(int x, int y, int w, int h) 
{
    if(auto *tex_node = get_texture_node()) tex_node->set_source_rect(x, y, w, h);
}

} // namespace cge
