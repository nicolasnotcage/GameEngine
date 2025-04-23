# Serialization and Deserialization Plan for MainScene

## Current State Analysis

The `MainScene` class currently implements serialization and deserialization through the `serialize()` and `deserialize()` methods. However, there's a significant imbalance between these two methods:

### Serialization (Current Implementation)
The `serialize()` method saves the following game state variables:
- Player position (x, y)
- NPC position (x, y)
- `waiting_to_clap_` flag
- `dialogue_completed_` flag
- `blue_witch_hidden_` flag
- `find_count_` counter
- `waiting_for_dialogue_` flag

### Deserialization (Current Implementation)
The `deserialize()` method only loads:
- Player position (x, y)

This imbalance means that when loading a saved game, most of the game state is not restored, which would result in an inconsistent game state.

## Deserialization Enhancement Plan

### 1. Update MainScene::deserialize() Method

We need to update the `deserialize()` method to load all the game state variables that are being saved:

```cpp
void MainScene::deserialize(Serializer& serializer)
{    
    // Deserialize player (witch) position
    auto &camera = root_.get_child<0>();
    auto &witch_transform = camera.get_child<2>();
    auto &blue_witch_transform = camera.get_child<1>();
    
    float player_x = 0.0f, player_y = 0.0f;
    float npc_x = 0.0f, npc_y = 0.0f;
    
    // Load player position
    if (serializer.read("player_x", player_x) && serializer.read("player_y", player_y)) 
    {          
        witch_transform.set_position(player_x, player_y);
    }
    
    // Load NPC position
    if (serializer.read("npc_x", npc_x) && serializer.read("npc_y", npc_y)) 
    {          
        blue_witch_transform.set_position(npc_x, npc_y);
    }
    
    // Load game state flags
    serializer.read("waiting_to_clap", waiting_to_clap_);
    serializer.read("dialogue_completed", dialogue_completed_);
    serializer.read("blue_witch_hidden", blue_witch_hidden_);
    serializer.read("find_count", find_count_);
    serializer.read("waiting_for_dialogue", waiting_for_dialogue_);
    
    // Update NPC visibility based on loaded state
    if (blue_witch_hidden_) {
        hide_npc();
    } else {
        show_npc();
    }
    
    // Update dialogue state based on loaded state
    if (waiting_for_dialogue_ && find_count_ > 0 && find_count_ <= 3) {
        show_dialogue_for_find(find_count_);
    }
}
```

### 2. Add Save Game Functionality with O Key

To add the ability to save the game by pressing the O key, we need to make the following changes:

#### 2.1 Add New Event Type in event.hpp

```cpp
enum class EventType
{
    // ... existing events ...
    KEY_DOWN_O,  // New event for O key press
};
```

#### 2.2 Update event.cpp to Handle O Key Press

```cpp
case SDL_EVENT_KEY_DOWN:
    switch(e.key.key)
    {
        // ... existing key mappings ...
        case SDLK_O: event_info.events[num_events++] = EventType::KEY_DOWN_O; break;
    }
```

#### 2.3 Add New Game Action in game_action.hpp

```cpp
enum class GameAction
{
    // ... existing actions ...
    SAVE_GAME,  // New action for saving the game
};
```

#### 2.4 Update input_interpreter.cpp to Map O Key to Save Game Action

```cpp
switch (events.events[i])
{
    // ... existing mappings ...
    case EventType::KEY_DOWN_O: action = GameAction::SAVE_GAME; break;
}
```

#### 2.5 Update handle_input_actions() in MainScene

```cpp
void MainScene::handle_input_actions()
{
    const GameActionList &actions = io_handler_->get_game_actions();
    for (uint8_t i = 0; i < actions.num_actions; i++)
    {
        if (actions.actions[i] == GameAction::TOGGLE_PAUSE)
        {
            // Push the pause menu
            SceneManager::get_instance()->push_scene_by_key("pause_menu");
            return; // Exit early to prevent further updates this frame
        }
        else if (actions.actions[i] == GameAction::SAVE_GAME)
        {
            // Save the game
            SaveManager::get_instance().save_game(this);
            
            // Provide feedback to the player (optional)
            std::cout << "Game saved successfully!" << std::endl;
        }
    }
}
```

## Implementation Steps

1. **Update Deserialization**: Implement the enhanced `deserialize()` method to load all game state variables.

2. **Add O Key Event Handling**:
   - Add `KEY_DOWN_O` to the `EventType` enum in event.hpp
   - Update event.cpp to handle O key press

3. **Add Save Game Action**:
   - Add `SAVE_GAME` to the `GameAction` enum in game_action.hpp
   - Update input_interpreter.cpp to map O key to save game action

4. **Update Input Handling**:
   - Modify `handle_input_actions()` in MainScene to handle the save game action

5. **Testing**:
   - Test saving the game at different stages (before finding NPC, after first find, etc.)
   - Test loading the game and verify that all game state is correctly restored
   - Verify that the NPC visibility and dialogue state are correctly restored

## Expected Behavior After Implementation

1. **Saving the Game**:
   - When the player presses the O key, the current game state will be saved
   - All relevant game state variables will be stored in the save file

2. **Loading the Game**:
   - When loading a saved game, all game state variables will be restored
   - The player and NPC will be positioned correctly
   - The NPC visibility state will be restored (hidden or visible)
   - The dialogue state will be restored
   - The find count will be restored
   - The game will continue from exactly where it was saved

This implementation will ensure that the game state is fully preserved when saving and loading, providing a seamless experience for the player.
