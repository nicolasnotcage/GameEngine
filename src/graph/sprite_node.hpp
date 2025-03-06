#ifndef GRAPH_SPRITE_NODE_HPP
#define GRAPH_SPRITE_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "graph/texture_node.hpp"

#include "platform/animation.hpp"
#include "platform/movement_controller.hpp"

#include <memory>
#include <unordered_map>

namespace cge
{

// A full-featured sprite node object. Intended to 
// provide an interface for sprite animation and 
// movement. 
class SpriteNode : public Node
{
  public:
    SpriteNode();
    ~SpriteNode() = default;

    // Overrides
    void init(SceneState &scene_state) override;
    void destroy() override;
    void draw(SceneState &scene_state) override;
    void update(SceneState &scene_state) override;

    // Texture management
    void         set_texture(TextureNode *texture);
    TextureNode *get_texture() const;

    // Animation delegation methods
    // TODO: This tighly couples sprites with animation sub-system
    void add_animation(const Animation &animation);
    void add_animation_with_texture(const Animation &animation, TextureNode *texture);
    void play(const std::string &animation_name);
    void pause();
    void resume();
    void reset();
    void set_playback_speed(float speed);
    void set_looping(bool looping);
    bool is_playing() const;
    const std::string &get_current_animation_name() const;

    // Movement methods
    // TODO: MovementController objects store a reference to the transform node
    // of the entities they control movement of. I imagine this could be
    // better.
    void set_player_controlled(TransformNode& transform_node);
    void set_path_controlled(TransformNode& transform_node, Path &path);
    MovementController *get_movement_controller() { return movement_controller_.get(); }

    // Animation integration with movement
    // TODO: This forces SpriteNodes to coordinate animation and movement systems. Is there a better way?
    void update_animation_for_movement();

  private:
    // Current texture and frame
    TextureNode *current_texture_;
    uint32_t     current_frame_id_;

    // Map of animation names to textures
    std::unordered_map<std::string, TextureNode *> animation_textures_;

    // Animation component
    std::unique_ptr<Animator> animator_;

    // Movement controller
    std::unique_ptr<MovementController> movement_controller_;
};

template <typename... ChildrenTs>
using SpriteNodeT = NodeT<SpriteNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_SPRITE_NODE_HPP
