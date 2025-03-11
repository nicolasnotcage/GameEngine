#include "graph/camera_node.hpp"
#include "platform/math.hpp"
#include "platform/io_handler.hpp"
#include "platform/config.hpp"
#include "platform/sdl.h"

#include <iostream>

namespace cge
{

void CameraNode::init(SceneState &scene_state) 
{ 
    // Init children
    init_children(scene_state); 
}

void CameraNode::destroy()
{
    destroy_children();
    clear_children();
}

void CameraNode::draw(SceneState &scene_state)
{
    // Update matrix stack, push transform, and draw children
    scene_state.matrix_stack.push();
    scene_state.matrix_stack.top() *= camera_.get_world_to_screen_matrix(cge::SCREEN_WIDTH, cge::SCREEN_HEIGHT);
    draw_children(scene_state);

    // Pop matrix stack and restore previous camera
    scene_state.matrix_stack.pop();
}

void CameraNode::update(SceneState &scene_state) 
{
    // Follow target if one is set
    if(follow_target_ && target_transform_)
    {
        // Get target position (world space)
        Matrix3 transform = target_transform_->get_transform();
        Vector2 target_position(transform.a[6], transform.a[7]);

        // Get current camera position
        Vector2 current_position = camera_.get_position();

        // Calculate interpolated position (smooth follow)
        float   lerp_factor = std::min(1.0f, follow_smoothness_ * scene_state.delta * 60.0f);
        Vector2 new_position =
            Vector2(current_position.x + (target_position.x - current_position.x) * lerp_factor,
                    current_position.y + (target_position.y - current_position.y) * lerp_factor);

        // Update camera position
        camera_.set_position(new_position.x, new_position.y);
    }

    // Handle camera controls from game actions
    if(scene_state.io_handler)
    {
        const GameActionList &action_list = scene_state.io_handler->get_game_actions();

        // Process all game actions that affect the camera
        for(uint8_t i = 0; i < action_list.num_actions; ++i)
        {
            // Calculate the move and zoom speeds based on delta time; change this to taste
            float move_speed = 5.0f * scene_state.delta;
            if(move_speed < 0.1f) move_speed = 1.0f;

            float zoom_speed = 1.1f;

            switch(action_list.actions[i])
            {
                case GameAction::CAMERA_ZOOM_IN: camera_.zoom(1.0f / zoom_speed); break;
                case GameAction::CAMERA_ZOOM_OUT: camera_.zoom(zoom_speed); break;

                // Handle mouse clicks if enabled
                case GameAction::MOUSE_BUTTON_LEFT:
                    if (print_on_click_)
                    {
                        // Get mouse position from SDL
                        float mouse_x;
                        float mouse_y;
                        SDL_GetMouseState(&mouse_x, &mouse_y);

                        // Convert to world coordinates
                        Vector2 world_pos = screen_to_world_coordinates(
                            mouse_x, mouse_y, cge::SCREEN_WIDTH, cge::SCREEN_HEIGHT);

                        // Print world coordinates
                        std::cout << "Click at screen position (" << mouse_x << ", " << mouse_y
                                  << ") maps to world position (" << world_pos.x << ", "
                                  << world_pos.y << ")" << std::endl;
                    }
                    break;

                default: break;
            }
        }
    }

    // Update children
    update_children(scene_state); 
}

Camera &CameraNode::get_camera() { return camera_; }

void CameraNode::set_target(TransformNode *target, bool follow)
{
    target_transform_ = target;
    follow_target_ = follow;
}

void CameraNode::set_follow_smoothness(float smoothness)
{
    follow_smoothness_ = std::max(0.001f, std::min(1.0f, smoothness));
}

bool CameraNode::is_following_target() const { return follow_target_ && target_transform_ != nullptr; }

// Print on click functions
void CameraNode::set_print_on_click(bool enabled) { print_on_click_ = enabled; }
bool CameraNode::get_print_on_click() const { return print_on_click_; }

Vector2 CameraNode::screen_to_world_coordinates(int screen_x, int screen_y, int screen_width, int screen_height) const
{
    // Normalize screen coordinates
    float normalized_x = static_cast<float>(screen_x) / screen_width;
    float normalized_y = static_cast<float>(screen_y) / screen_height;

    // Convert to [-1, 1]
    float device_x = 2.0f * normalized_x - 1.0f;
    float device_y = 2.0f * normalized_y - 1.0f;

    // Scale to camera dimensions
    float world_x = camera_.get_position().x + (device_x * camera_.get_width() / 2.0f);
    float world_y = camera_.get_position().y + (device_y * camera_.get_height() / 2.0f);

    return Vector2(world_x, world_y);
}

} // namespace cge
