#ifndef GRAPH_TEXT_NODE_HPP
#define GRAPH_TEXT_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "graph/texture_node.hpp"

#include "platform/io_handler.hpp"

#include <memory>
#include <vector>

namespace cge
{

// A full-featured sprite node object. Intended to 
// provide an interface for sprite animation and 
// movement. 
class TextNode : public Node
{
  public:
    TextNode() = default;
    ~TextNode() = default;

    // Overrides
    void init(SceneState &scene_state) override;
    void destroy() override;
    void draw(SceneState &scene_state) override;
    void update(SceneState &scene_state) override;

    void set_should_render(bool should_render) { is_rendered_ = should_render; }
    bool is_rendered() const { return is_rendered_; }
    void push_texture(TextureNode* texture) { text_textures_.push_back(texture); }
    void clear_textures() { text_textures_.clear(); }

  private:
    std::vector<TextureNode*> text_textures_{};
    uint16_t curr_text{0};
    bool is_rendered_{false};
};

template <typename... ChildrenTs>
using TextNodeT = NodeT<TextNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_TEXT_NODE_HPP
