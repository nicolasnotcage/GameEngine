#include "graph/camera_node.hpp"
#include "platform/math.hpp"
#include "platform/io_handler.hpp"
#include "platform/config.hpp"

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
                case GameAction::CAMERA_MOVE_UP: camera_.move(0.0f, move_speed); break;

                case GameAction::CAMERA_MOVE_DOWN: camera_.move(0.0f, -move_speed); break;

                case GameAction::CAMERA_MOVE_LEFT: camera_.move(-move_speed, 0.0f); break;

                case GameAction::CAMERA_MOVE_RIGHT: camera_.move(move_speed, 0.0f); break;

                case GameAction::CAMERA_ZOOM_IN: camera_.zoom(1.0f / zoom_speed); break;

                case GameAction::CAMERA_ZOOM_OUT: camera_.zoom(zoom_speed); break;

                default: break;
            }
        }
    }

    // Update children
    update_children(scene_state); 
}

Camera &CameraNode::get_camera() { return camera_; }

} // namespace cge
