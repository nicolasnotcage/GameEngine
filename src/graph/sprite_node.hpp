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

class SpriteNode : public Node
{
  public:
    SpriteNode();
    ~SpriteNode() = default;

    void init(SceneState &scene_state) override;
    void destroy() override;
    void draw(SceneState &scene_state) override;
    void update(SceneState &scene_state) override;

    // Texture management
    void         set_texture(TextureNode *texture);
    TextureNode *get_texture() const;

    // Animation delegation methods
    void               add_animation(const Animation &animation);
    void               add_animation_with_texture(const Animation &animation, TextureNode *texture);
    void               play(const std::string &animation_name);
    void               pause();
    void               resume();
    void               reset();
    void               set_playback_speed(float speed);
    void               set_looping(bool looping);
    bool               is_playing() const;
    const std::string &get_current_animation_name() const;

    // Movement methods
    void set_player_controlled(TransformNode& transform_node);
    void set_path_controlled(TransformNode& transform_node, Path &path);
    MovementController *get_movement_controller() { return movement_controller_.get(); }

    // Animation integration with movement
    void update_animation_for_movement();

  private:
    // Current texture and frame
    TextureNode *current_texture_;
    uint32_t     current_frame_id_;

    // Map of animation names to textures
    std::unordered_map<std::string, TextureNode *> animation_textures_;

    // Animation component - composition instead of inheritance
    std::unique_ptr<Animator> animator_;

    // Movement controller
    std::unique_ptr<MovementController> movement_controller_;

    // For animation damping
    float animation_min_duration_{0.3f};
    float current_animation_time_{0.0f};
};

template <typename... ChildrenTs>
using SpriteNodeT = NodeT<SpriteNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_SPRITE_NODE_HPP
