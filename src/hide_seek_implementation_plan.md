# Hide and Seek Gameplay Demo Implementation Plan

## Current State of the Game

The game currently has:
- A blue witch (NPC) with idle and run animations
- A white witch (player character) with idle and run animations
- A dialogue system (TextNode) that displays text and advances with spacebar
- A collision system that detects collisions between entities
- An audio system that can play sounds (including 3D positional audio)
- A pathing system for NPCs (as seen in old_static_scene)

## Requirements for Hide and Seek Demo

1. **Initial Setup**:
   - Blue witch should start at transform position (0, 2), to the right of the player
   - Player should go through the first set of dialogue text using spacebar

2. **Blue Witch Movement**:
   - After dialogue completion, blue witch should path straight down and then disappear
   - Blue witch should be relocated to the top left of the map but be invisible
   - Need to implement invisibility by using a transparent PNG texture

3. **Gameplay Mechanics**:
   - When player presses F to whistle, the blue witch should clap in response
   - When player runs into the blue witch's collider, the witch should:
     - Become visible again (set texture back to normal idle)
     - Trigger a new line of dialogue (second TextNode in scene graph)

## Implementation Plan

### Step 1: Understand the Current Architecture
- The main_scene.cpp already has the blue witch and player set up
- TextNode is already implemented and can be advanced with spacebar
- NPC pathing is demonstrated in old_static_scene.cpp
- Collision detection and response is already implemented
- Audio system is set up with the ability to play sounds

### Step 2: Create Transparent Texture for Blue Witch
- Create or modify a transparent version of the blue witch texture
- Add this texture to the game resources
- Load this texture in the MainScene::init method

### Step 3: Modify Initial Positions
- Update the blue witch's initial position to (0, 2)
- Ensure the player is positioned appropriately

### Step 4: Implement Dialogue Completion Detection
- Modify the TextNode to signal when all dialogue has been completed
- Add a callback or state variable to track when dialogue is finished

### Step 5: Implement Blue Witch Pathing and Disappearance
- Create a Path for the blue witch to move downward
- Set the blue witch to be path-controlled after dialogue completion
- When the path is completed, change the blue witch's texture to the transparent one
- Relocate the blue witch to the top left of the map

### Step 6: Implement Whistle Response
- The game already has a PLAYER_WHISTLE action mapped to the F key
- Modify the handle_audio method to make the blue witch clap when the player whistles
- This can leverage the existing audio component on the blue witch

### Step 7: Implement Collision Detection for Finding the Witch
- Add logic to the entity collision response to detect when the player collides with the blue witch
- When collision occurs, change the blue witch's texture back to the normal idle texture
- Create and trigger a second TextNode for the "found you" dialogue

### Step 8: Testing and Refinement
- Test the complete gameplay flow
- Refine timing, positions, and interactions as needed
- Ensure all game mechanics work as expected

## Detailed Implementation Steps

1. **Update Blue Witch Position and Create Transparent Texture**
   ```cpp
   // In MainScene::init
   blue_witch_transform.right_translate(0.0f, 2.0f); // Position at (0, 2)
   
   // Add transparent texture
   blue_witch_transparent_texture_.set_filepath("images/blue_witch/B_witch_transparent.png");
   blue_witch_transparent_texture_.set_blend(true);
   blue_witch_transparent_texture_.set_blend_alpha(0); // Fully transparent
   blue_witch_transparent_texture_.define_grid(1, 6, 32, 48);
   blue_witch_transparent_texture_.init(scene_state_);
   ```

2. **Create Path for Blue Witch**
   ```cpp
   // In MainScene class declaration
   Path blue_witch_path_;
   bool dialogue_completed_{false};
   bool blue_witch_hidden_{false};
   
   // In MainScene::init or a separate setup method
   blue_witch_path_.add_point(0.0f, 2.0f, 0.5f);  // Start position
   blue_witch_path_.add_point(0.0f, 5.0f, 0.5f);  // Move down
   blue_witch_path_.set_looping(false);
   ```

3. **Detect Dialogue Completion and Start Pathing**
   ```cpp
   // In MainScene::update
   auto &camera = root_.get_child<0>();
   auto &text_transform = camera.get_child<2>().get_child<1>();
   auto &text_node = text_transform.get_child<0>();
   auto &blue_witch_transform = camera.get_child<1>();
   
   // Check if dialogue is not rendered anymore (completed)
   if (!dialogue_completed_ && !text_node.is_rendered()) {
       dialogue_completed_ = true;
       blue_witch_transform.set_path_controlled(blue_witch_path_);
   }
   
   // Check if path is completed and witch should be hidden
   if (dialogue_completed_ && !blue_witch_hidden_ && 
       !blue_witch_transform.is_moving() && 
       blue_witch_transform.get_position_y() >= 5.0f) {
       // Hide witch and move to top left
       auto &blue_witch_sprite = blue_witch_transform.get_child<0>();
       blue_witch_sprite.set_texture(&blue_witch_transparent_texture_);
       blue_witch_transform.set_position(-15.0f, -7.0f);
       blue_witch_hidden_ = true;
   }
   ```

4. **Implement Whistle Response**
   ```cpp
   // In MainScene::handle_audio
   const GameActionList &actions = io_handler_->get_game_actions();
   for(uint8_t i = 0; i < actions.num_actions; i++) {
       if(actions.actions[i] == GameAction::PLAYER_WHISTLE) {
           // Play witch whistle sound
           if(auto *witch_audio = witch_transform.get_audio_component()) {
               witch_audio->play(1.0f);
           }
           
           // Make blue witch clap in response if hidden
           if (blue_witch_hidden_ && auto* blue_witch_audio = blue_witch_transform.get_audio_component()) {
               blue_witch_audio->play(1.0f);
           }
           break;
       }
   }
   ```

5. **Implement Collision Detection for Finding the Witch**
   ```cpp
   // Add a second TextNode for "found you" dialogue
   // In MainScene class declaration
   TextureNode found_you_text_;
   bool witch_found_{false};
   
   // In MainScene::init
   found_you_text_.set_filepath("images/game_text/found_you.png");
   found_you_text_.set_blend(true);
   found_you_text_.set_blend_alpha(200);
   found_you_text_.init(scene_state_);
   
   // Configure second text node
   auto &found_text_transform = root_.get_child<0>().get_child<2>().get_child<2>();
   auto &found_text_node = found_text_transform.get_child<0>();
   found_text_transform.right_translate(0, 1.8f);
   found_text_transform.right_scale(3.0f, 1.0f);
   found_text_node.push_texture(&found_you_text_);
   found_text_node.set_should_render(false);
   
   // In collision response
   collision_system_.register_entity_response([this, &blue_witch_transform, &witch_transform](TransformNode* entity_a, TransformNode* entity_b) {
       // Check for witches collision
       bool witches_colliding =
           (entity_a == &witch_transform && entity_b == &blue_witch_transform) ||
           (entity_b == &witch_transform && entity_a == &blue_witch_transform);
       
       // Handle witch found logic
       if (witches_colliding && blue_witch_hidden_ && !witch_found_) {
           // Make blue witch visible again
           auto &blue_witch_sprite = blue_witch_transform.get_child<0>();
           blue_witch_sprite.set_texture(&blue_witch_idle_texture_);
           blue_witch_sprite.play("idle");
           
           // Show "found you" dialogue
           auto &found_text_node = root_.get_child<0>().get_child<2>().get_child<2>().get_child<0>();
           found_text_node.set_should_render(true);
           
           // Play found sound
           AudioEngine::get_instance()->play_sound("creepy_ha", 1.0f);
           
           witch_found_ = true;
       }
   });
   ```

## Conclusion

This implementation plan outlines the steps needed to create a hide and seek gameplay demo using the existing game engine architecture. The plan leverages the current systems for pathing, collision detection, audio, and dialogue to create an interactive gameplay experience.

The key additions are:
1. A transparent texture for the blue witch
2. Path-controlled movement for the blue witch
3. Logic to detect dialogue completion
4. Whistle response mechanics
5. Collision detection for finding the hidden witch
6. A second dialogue for when the witch is found

By following this plan, we can create an engaging hide and seek demo that showcases the capabilities of the game engine.
