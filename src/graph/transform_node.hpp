#ifndef GRAPH_TRANSFORM_NODE_HPP
#define GRAPH_TRANSFORM_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "platform/math.hpp"
#include "platform/path.hpp"

#include <memory>

namespace cge
{

// Forward declarations
class SpriteNode;
class MovementController;
enum class MoveDirection;
class CollisionComponent;
class CircleCollisionComponent;
class AABBCollisionComponent;

class TransformNode : public Node
{
  public:
    TransformNode() = default;
    ~TransformNode() = default;

    // Overrides
    void init(SceneState &scene_state) override;
    void destroy() override;
    void draw(SceneState &scene_state) override;
    void update(SceneState &scene_state) override;

    // Transformation functions
    void set_identity();
    void left_scale(float x, float y);
    void right_scale(float x, float y);
    void left_rotate_degrees(float angle_deg);
    void right_rotate_degrees(float angle_deg);
    void left_rotate(float rad_deg);
    void right_rotate(float rad_deg);
    void left_translate(float x, float y);
    void right_translate(float x, float y);

    Matrix3 &get_transform() { return transform_; }

    // Movement controller methods
    void                set_player_controlled();
    void                set_path_controlled(Path &path);
    bool                is_moving() const;
    
    
    // Return movement direction
    MoveDirection get_move_direction() const;

    // Return if facing left
    bool is_facing_left() const;

    // Connect a sprite to this transform
    void set_associated_sprite(SpriteNode *sprite);

    // Collision component methods
    CircleCollisionComponent *add_circle_collider(float radius);
    AABBCollisionComponent   *add_aabb_collider(const Vector2 &min, const Vector2 &max);
    CollisionComponent       *get_collision_component() const { return collision_component_; }

private:
    Matrix3 transform_;
    std::unique_ptr<MovementController> movement_controller_;
    SpriteNode                         *associated_sprite_{nullptr};

    // Collision component
    CollisionComponent *collision_component_{nullptr};
};

template <typename... ChildrenTs>
using TransformNodeT = NodeT<TransformNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_TRANSFORM_NODE_HPP
