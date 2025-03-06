/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef PLATFORM_MOVEMENT_CONTROLLER_HPP
#define PLATFORM_MOVEMENT_CONTROLLER_HPP

#include "graph/transform_node.hpp"
#include "graph/scene_state.hpp"
#include "platform/game_action.hpp"
#include "platform/path.hpp"

namespace cge
{

enum class MoveDirection
{
	NONE, 
	UP,
	DOWN,
	LEFT,
	RIGHT
};

// Base class for movement controllers
class MovementController
{
  public:
  MovementController(TransformNode &transform_node) :
      transform_node_(transform_node),
      speed_(5.0f),
      is_moving_(false),
      current_direction_(MoveDirection::NONE)
      {}

  virtual ~MovementController() = default;

  // Update movement, which will be overwritten by derived classes.
  virtual void update(SceneState &scene_state) = 0;

  // Set movement speed
  void set_speed(float speed) { speed_ = speed; }

  // Get current direction 
  MoveDirection get_direction() const { return current_direction_; }

  // Flag getters
  bool is_moving() const { return is_moving_; }
  bool is_facing_left() const { return facing_left_; }

protected: 
	TransformNode &transform_node_;
	float          speed_;
    bool           is_moving_;
    MoveDirection  current_direction_;
    bool           facing_left_ = false;

    void update_sprite_orientation();
};

// Player movement controller
class PlayerController : public MovementController
{
  public:
    PlayerController(TransformNode &transform_node) : MovementController(transform_node) {}

    // Update movement based on player input
    void update(SceneState &scene_state) override;
};

// NPC movement; driven by path-based automation
class PathController : public MovementController
{
public: 
    PathController(TransformNode &transform_node) :
    MovementController(transform_node), current_point_index_(0), pause_timer_(0.0f)
    {}

    // Update movement along path
    void update(SceneState &scene_state) override;

    // Set the path to follow
    void set_path(const Path &path) { path_ = path; }

private:
    Path   path_;
    size_t current_point_index_;
    float  pause_timer_;

    // Move towards current target point
    void move_towards_target(float delta);
};

} // namespace cge

#endif // PLATFORM_MOVEMENT_CONTROLLER_HPP
