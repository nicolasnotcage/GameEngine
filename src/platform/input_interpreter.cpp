#include "platform/input_interpreter.hpp"
#include "platform/event.hpp"
#include "platform/game_action.hpp"

namespace cge
{

// Translates raw SDL events into a list of game-specifc actions. 
GameActionList InputInterpreter::interpret_events(const SDLEventInfo &events)
{
    GameActionList action_list;
    action_list.num_actions = 0;

    for (uint8_t i = 0; i < events.num_events; i++)
    {
        if(action_list.num_actions >= GameActionList::MAX_ACTIONS) break;

        // Initialize action to null
        GameAction action = GameAction::NONE;

        switch (events.events[i])
        {
            // Givens
            case EventType::QUIT: action = GameAction::QUIT; break;
            case EventType::PLAY_SOUND: action = GameAction::PLAY_SOUND; break;
            case EventType::TOGGLE_MUSIC: action = GameAction::TOGGLE_MUSIC; break;

            // WASD movement
            case EventType::KEY_HELD_W: action = GameAction::PLAYER_MOVE_UP; break;
            case EventType::KEY_HELD_S: action = GameAction::PLAYER_MOVE_DOWN; break;
            case EventType::KEY_HELD_A: action = GameAction::PLAYER_MOVE_LEFT; break;
            case EventType::KEY_HELD_D: action = GameAction::PLAYER_MOVE_RIGHT; break;

            // Zoom behavior
            case EventType::KEY_DOWN_MINUS: 
            case EventType::MOUSE_WHEEL_DOWN: action = GameAction::CAMERA_ZOOM_OUT; break;
            case EventType::KEY_DOWN_PLUS:
            case EventType::MOUSE_WHEEL_UP: action = GameAction::CAMERA_ZOOM_IN; break;

            // For clicking to get world coordinates
            case EventType::MOUSE_BUTTON_LEFT: action = GameAction::MOUSE_BUTTON_LEFT; break;

            // Skip over unbound events
            default: continue;
        }

        if (action != GameAction::NONE) action_list.actions[action_list.num_actions++] = action;
    }

    return action_list;
}

} // namespace cge

