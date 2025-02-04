#ifndef GRAPH_SPRITE_NODE_HPP
#define GRAPH_SPRITE_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "graph/texture_node.hpp"
#include "graph/geometry_node.hpp"
namespace cge
{

/**
* Generic sprite node class. Provides the basic functionality expected of an engine node. 
* Primary functionality is defined within the template class. 
**/
class SpriteNode : public Node
{
public:
    SpriteNode() = default;
    ~SpriteNode() = default;

    // Overrides
    void init(SceneState &scene_state) override;
    void destroy() override;
    void draw(SceneState &scene_state) override;

};

/**
* Templated sprite node class that provides wrapper functions for texture and geometry nodes.
* 
* Assumes:
*    1) A sprite node's first child is a texture node
*    2) A texture node's first child is a geometry node 
* 
* Is there some way to make this more dynamic? Or is this a consequence of the static approach?
* 
* TODO: Add sprite sheet support
**/
template <typename... ChildrenTs>
class SpriteNodeT : public NodeT<SpriteNode, ChildrenTs...>
{
  public:
    // Wrapped texture node functions
    void set_filepath(const std::string &path)
    {
        auto &tex_node = this->template get_child<0>();
        tex_node.set_filepath(path);
    }

    void set_blend(bool blend)
    {
        auto &tex_node = this->template get_child<0>();
        tex_node.set_blend(blend);
    }

    void set_blend_alpha(uint8_t alpha)
    {
        auto &tex_node = this->template get_child<0>();
        tex_node.set_blend_alpha(alpha);
    }

    void set_color_mods(const uint8_t mods[3])
    {
        auto &tex_node = this->template get_child<0>();
        tex_node.set_color_mods(mods);
    }

    // Wrapped geometry node functions
    void set_top_left(float x, float y)
    {
        auto &tex_node = this->template get_child<0>();
        auto &geo_node = tex_node.template get_child<0>();
        geo_node.set_top_left(x, y);
    }

    void set_top_right(float x, float y)
    {
        auto &tex_node = this->template get_child<0>();
        auto &geo_node = tex_node.template get_child<0>();
        geo_node.set_top_right(x, y);
    }

    void set_bottom_left(float x, float y)
    {
        auto &tex_node = this->template get_child<0>();
        auto &geo_node = tex_node.template get_child<0>();
        geo_node.set_bottom_left(x, y);
    }

    // Sprite sheet fields and methods
    void set_sprite_sheet_info(int columns, int rows)
    {
        sheet_columns_ = columns;
        sheet_rows_ = rows;
    }

    void set_frame(int frame)
    { 
        current_frame_ = frame;
        update_sub_rect();
    }

  protected:
    // Compute the sub rect each time the frame is updated
    void update_sub_rect() 
    { 
        auto &tex_node = this->template get_child<0>();

        // Get width and height of texture node
        int tex_width = tex_node.width();
        int tex_height = tex_node.height();

        // Return if sheets and columns are zero
        if (sheet_columns_ == 0 || sheet_rows_ == 0) return;

        int frame_width = tex_width / sheet_columns_;
        int frame_height = tex_height / sheet_rows_;

        // Clamp current frame if out of range
        int total_frames = sheet_columns_ * sheet_rows_;
        if(current_frame_ >= total_frames) current_frame_ = total_frames - 1;
        if(current_frame_ < 0) current_frame_ = 0;

        int col = current_frame_ % sheet_columns_;
        int row = current_frame_ / sheet_columns_;

        int sx = col * frame_width;
        int sy = row * frame_height;

        // Set the source rectangle within the texture node
        tex_node.set_source_rect(sx, sy, frame_w, frame_h);
    }
  private:
    int sheet_columns_{0};
    int sheet_rows_{0};
    int current_frame_{0};
};

// Define sprite types
using BasicSprite = SpriteNodeT<TextureNodeT<GeometryNodeT<>>>;

} // namespace cge

#endif // GRAPH_SPRITE_NODE_HPP
