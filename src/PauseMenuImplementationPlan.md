# Pause Menu Implementation Plan

## Overview

This document outlines the implementation plan for adding a pause menu to the game. The pause menu will be triggered by pressing the Escape key while playing the game in the main scene. It will overlay on top of the main scene and provide three options: Resume, Main Menu, and Exit.

## Current Structure Analysis

After reviewing the codebase, I've identified the following key components:

1. **Scene Management**:
   - The game uses a scene stack managed by `SceneManager`
   - Scenes can be pushed, popped, and replaced
   - The top scene in the stack is the active scene that receives updates and renders

2. **UI System**:
   - The game has a `UIButton` class for interactive buttons
   - Buttons can have different states (normal, hover, pressed) with different textures
   - Buttons can execute callbacks when clicked

3. **Main Menu Implementation**:
   - The main menu is implemented as a scene (`MainMenuScene`)
   - It uses a node graph structure with a camera, background, title, and buttons
   - Each button has a callback that performs an action when clicked

4. **Input Handling**:
   - The game uses an `IoHandler` to process input events
   - The Escape key is currently mapped to quit the game
   - This mapping will need to be modified to trigger the pause menu instead

5. **Serialization**:
   - The game has a serialization system for saving and loading game state
   - The `SaveManager` handles saving and loading game state
   - Each scene can implement `serialize` and `deserialize` methods

## Pause Menu Design

The pause menu will be implemented as a new scene (`PauseMenuScene`) that will be pushed onto the scene stack when the Escape key is pressed. The pause menu will have the following features:

1. **Visual Design**:
   - Half the width and 3/4 the height of the screen
   - Semi-transparent background to show the main scene underneath
   - Three buttons: Resume, Main Menu, and Exit

2. **Functionality**:
   - **Resume**: Pop the pause menu from the scene stack to return to the main scene
   - **Main Menu**: Serialize the main scene state, pop the pause menu, and replace the main scene with the main menu
   - **Exit**: Serialize the main scene state and exit the game

3. **Input Handling**:
   - Modify the input interpreter to map the Escape key to a new game action (e.g., `TOGGLE_PAUSE`)
   - Update the main scene to push the pause menu when this action is detected

## Implementation Steps

1. **Create Pause Menu Scene**:
   - Implement `PauseMenuScene` class based on the existing `MainMenuScene`
   - Set up the node graph structure with a camera, background, and buttons
   - Implement button callbacks for Resume, Main Menu, and Exit

2. **Update Input Handling**:
   - Modify `EventType` to include a new event for the Escape key
   - Update `InputInterpreter` to map the Escape key to a new `TOGGLE_PAUSE` game action
   - Ensure the Escape key no longer directly triggers a quit action

3. **Update Main Scene**:
   - Modify the main scene to check for the `TOGGLE_PAUSE` action
   - When detected, push the pause menu scene onto the stack

4. **Register Pause Menu Scene**:
   - Update `main.cpp` to register the pause menu scene with the scene manager

5. **Implement Button Callbacks**:
   - **Resume**: `SceneManager::pop_scene()`
   - **Main Menu**: 
     ```cpp
     SaveManager::get_instance().save_game(current_scene);
     SceneManager::get_instance()->pop_scene(); // Remove pause menu
     SceneManager::get_instance()->replace_scene_by_key("main_menu");
     ```
   - **Exit**: 
     ```cpp
     SaveManager::get_instance().save_game(current_scene);
     SDL_Event quit_event;
     quit_event.type = SDL_EVENT_QUIT;
     SDL_PushEvent(&quit_event);
     ```

## Technical Considerations

1. **Scene Stacking**:
   - When the pause menu is pushed onto the stack, the main scene will be paused but still visible
   - The pause menu will render on top of the main scene
   - This requires the pause menu background to be semi-transparent

2. **Input Focus**:
   - While the pause menu is active, input should only affect the pause menu, not the main scene
   - This is handled automatically by the scene stack system

3. **Serialization**:
   - The main scene state should be serialized before navigating to the main menu or exiting
   - This ensures the game state can be restored later

4. **Performance**:
   - The pause menu should be lightweight to ensure smooth transitions
   - Consider using simple graphics and minimal animations

## Conclusion

This implementation plan provides a structured approach to adding a pause menu to the game. By leveraging the existing scene management and UI systems, we can create a seamless pause experience that allows players to resume, return to the main menu, or exit the game.

The pause menu will enhance the player experience by providing standard game control options and ensuring game state is properly saved when navigating away from the main scene.
