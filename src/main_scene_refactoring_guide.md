# MainScene Refactoring Guide

## Introduction

This document provides a detailed guide for refactoring the `MainScene` class to improve its design, maintainability, and extensibility. The current implementation, while functional, has several architectural issues that should be addressed. This guide offers concrete examples and step-by-step instructions for implementing the recommended changes.

## Current Issues

The `MainScene` class suffers from several design issues:

1. **God Class Anti-pattern**: The class has grown to handle too many responsibilities
2. **Tight Coupling**: Game logic is tightly coupled with engine systems
3. **Brittle Scene Graph Navigation**: Hard-coded indices for accessing nodes
4. **State Management**: Game state is managed through numerous boolean flags
5. **Lack of Abstraction**: Missing higher-level abstractions for common game elements

## Refactoring Plan

### Phase 1: Extract Character Management

#### Step 1: Create Character Base Class

```cpp
// character.hpp
#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "graph/transform_node.hpp"
#include "graph/sprite_node.hpp"
#include "platform/path.hpp"

namespace cge
{

class Character
{
public:
    Character(TransformNode* transform_node);
    virtual ~Character() = default;
    
    virtual void update(double delta);
    
    void set_position(float x, float y);
    float get_position_x() const;
    float get_position_y() const;
    
    void set_animation(const std::string& animation_name);
    void set_texture(TextureNode* texture);
    
    TransformNode* get_transform_node() { return transform_node_; }
    
protected:
    TransformNode* transform_node_;
    SpriteNode* sprite_node_;
};

} // namespace cge

#endif // CHARACTER_HPP
```

```cpp
// character.cpp
#include "character.hpp"

namespace cge
{

Character::Character(TransformNode* transform_node)
    : transform_node_(transform_node)
{
    // Get the sprite node (assuming it's the first child)
    sprite_node_ = dynamic_cast<SpriteNode*>(&transform_node->get_child<0>());
}

void Character::update(double delta)
{
    // Base update logic
}

void Character::set_position(float x, float y)
{
    transform_node_->set_position(x, y);
}

float Character::get_position_x() const
{
    return transform_node_->get_position_x();
}

float Character::get_position_y() const
{
    return transform_node_->get_position_y();
}

void Character::set_animation(const std::string& animation_name)
{
    if (sprite_node_) {
        sprite_node_->play(animation_name);
    }
}

void Character::set_texture(TextureNode* texture)
{
    if (sprite_node_) {
        sprite_node_->set_texture(texture);
    }
}

} // namespace cge
```

#### Step 2: Create NPC Class

```cpp
// npc.hpp
#ifndef NPC_HPP
#define NPC_HPP

#include "character.hpp"

namespace cge
{

class NPC : public Character
{
public:
    NPC(TransformNode* transform_node);
    
    void update(double delta) override;
    
    void hide();
    void show();
    bool is_hidden() const;
    
    void set_path(const Path& path);
    bool is_moving() const;
    
private:
    bool hidden_ = false;
    Path current_path_;
};

} // namespace cge

#endif // NPC_HPP
```

```cpp
// npc.cpp
#include "npc.hpp"

namespace cge
{

NPC::NPC(TransformNode* transform_node)
    : Character(transform_node)
{
}

void NPC::update(double delta)
{
    Character::update(delta);
    
    // NPC-specific update logic
}

void NPC::hide()
{
    if (sprite_node_) {
        sprite_node_->set_auto_animation_enabled(false);
        // Assuming transparent texture is set elsewhere
        sprite_node_->play("hidden");
    }
    hidden_ = true;
}

void NPC::show()
{
    if (sprite_node_) {
        sprite_node_->set_auto_animation_enabled(true);
        // Assuming idle texture is set elsewhere
        sprite_node_->play("idle");
    }
    hidden_ = false;
}

bool NPC::is_hidden() const
{
    return hidden_;
}

void NPC::set_path(const Path& path)
{
    current_path_ = path;
    transform_node_->set_path_controlled(current_path_);
}

bool NPC::is_moving() const
{
    return transform_node_->is_moving();
}

} // namespace cge
```

#### Step 3: Create Player Class

```cpp
// player.hpp
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "character.hpp"

namespace cge
{

class Player : public Character
{
public:
    Player(TransformNode* transform_node);
    
    void update(double delta) override;
    void whistle();
    
private:
    // Player-specific properties
};

} // namespace cge

#endif // PLAYER_HPP
```

```cpp
// player.cpp
#include "player.hpp"
#include "platform/audio_component.hpp"

namespace cge
{

Player::Player(TransformNode* transform_node)
    : Character(transform_node)
{
    // Set player as player-controlled
    transform_node_->set_player_controlled();
}

void Player::update(double delta)
{
    Character::update(delta);
    
    // Player-specific update logic
}

void Player::whistle()
{
    if (auto* audio = transform_node_->get_audio_component()) {
        audio->play(1.0f);
    }
}

} // namespace cge
```

#### Step 4: Update MainScene to Use Character Classes

```cpp
// In MainScene.hpp
private:
    std::unique_ptr<Player> player_;
    std::unique_ptr<NPC> blue_witch_;
```

```cpp
// In MainScene.cpp, setup_characters()
void MainScene::setup_characters()
{
    auto &camera = root_.get_child<0>();
    auto &blue_witch_transform = camera.get_child<1>();
    auto &witch_transform = camera.get_child<2>();

    // Create character objects
    blue_witch_ = std::make_unique<NPC>(&blue_witch_transform);
    player_ = std::make_unique<Player>(&witch_transform);
    
    // Position blue witch and configure path
    blue_witch_->set_position(4.0f, 1.0f);
    blue_witch_path_.add_point(4.0f, 1.0f, 0.5f);  // Start position
    blue_witch_path_.add_point(4.0f, 2.5f, 0.5f);  // Move down
    blue_witch_path_.set_looping(false);
    
    // Scale characters
    blue_witch_transform.right_scale(2.0f, 2.0f);
    witch_transform.right_scale(3.0f, 3.0f);

    // Set player position if no save file exists
    if (!SaveManager::get_instance().save_exists()) {
        player_->set_position(1.0f, 0.0f);
    }

    // Set camera to follow player
    camera.set_target(&witch_transform, true);
    camera.set_follow_smoothness(1.0f);
}
```

### Phase 2: Extract Dialogue System

#### Step 1: Create DialogueManager Class

```cpp
// dialogue_manager.hpp
#ifndef DIALOGUE_MANAGER_HPP
#define DIALOGUE_MANAGER_HPP

#include "graph/text_node.hpp"
#include "graph/texture_node.hpp"
#include <functional>
#include <vector>
#include <string>

namespace cge
{

class DialogueManager
{
public:
    enum class DialogueState {
        INACTIVE,
        INTRO,
        FIRST_FIND,
        SECOND_FIND,
        THIRD_FIND,
        COMPLETED
    };
    
    DialogueManager();
    ~DialogueManager() = default;
    
    void init(SceneState& scene_state);
    void update();
    
    void register_text_node(const std::string& id, TextNode* node);
    void register_texture(const std::string& id, TextureNode* texture);
    
    void show_dialogue(DialogueState state);
    void hide_all_dialogue();
    
    bool is_dialogue_active() const;
    DialogueState get_current_state() const;
    
    void register_on_dialogue_completed_callback(std::function<void(DialogueState)> callback);
    
private:
    struct DialogueEntry {
        std::string id;
        TextNode* node;
        bool active = false;
    };
    
    std::vector<DialogueEntry> dialogue_entries_;
    std::unordered_map<std::string, TextureNode*> textures_;
    
    DialogueState current_state_ = DialogueState::INACTIVE;
    bool dialogue_active_ = false;
    
    std::function<void(DialogueState)> on_dialogue_completed_;
};

} // namespace cge

#endif // DIALOGUE_MANAGER_HPP
```

```cpp
// dialogue_manager.cpp
#include "dialogue_manager.hpp"

namespace cge
{

DialogueManager::DialogueManager()
{
}

void DialogueManager::init(SceneState& scene_state)
{
    // Initialize dialogue system
}

void DialogueManager::update()
{
    // Check if any active dialogue has been dismissed
    bool any_active = false;
    
    for (auto& entry : dialogue_entries_) {
        if (entry.active && entry.node) {
            if (!entry.node->is_rendered()) {
                entry.active = false;
                
                // Dialogue was dismissed, trigger callback
                if (on_dialogue_completed_ && !any_active) {
                    on_dialogue_completed_(current_state_);
                    dialogue_active_ = false;
                }
            } else {
                any_active = true;
            }
        }
    }
    
    dialogue_active_ = any_active;
}

void DialogueManager::register_text_node(const std::string& id, TextNode* node)
{
    if (node) {
        DialogueEntry entry;
        entry.id = id;
        entry.node = node;
        entry.active = false;
        dialogue_entries_.push_back(entry);
    }
}

void DialogueManager::register_texture(const std::string& id, TextureNode* texture)
{
    if (texture) {
        textures_[id] = texture;
    }
}

void DialogueManager::show_dialogue(DialogueState state)
{
    current_state_ = state;
    dialogue_active_ = true;
    
    // Hide all dialogues first
    hide_all_dialogue();
    
    // Show the appropriate dialogue based on state
    switch (state) {
        case DialogueState::INTRO:
            for (auto& entry : dialogue_entries_) {
                if (entry.id == "intro") {
                    entry.active = true;
                    entry.node->set_should_render(true);
                    break;
                }
            }
            break;
            
        case DialogueState::FIRST_FIND:
            for (auto& entry : dialogue_entries_) {
                if (entry.id == "first_find") {
                    entry.active = true;
                    entry.node->set_should_render(true);
                    break;
                }
            }
            break;
            
        // ... other cases ...
            
        default:
            break;
    }
}

void DialogueManager::hide_all_dialogue()
{
    for (auto& entry : dialogue_entries_) {
        entry.active = false;
        if (entry.node) {
            entry.node->set_should_render(false);
        }
    }
}

bool DialogueManager::is_dialogue_active() const
{
    return dialogue_active_;
}

DialogueManager::DialogueState DialogueManager::get_current_state() const
{
    return current_state_;
}

void DialogueManager::register_on_dialogue_completed_callback(std::function<void(DialogueState)> callback)
{
    on_dialogue_completed_ = callback;
}

} // namespace cge
```

#### Step 2: Update MainScene to Use DialogueManager

```cpp
// In MainScene.hpp
private:
    std::unique_ptr<DialogueManager> dialogue_manager_;
```

```cpp
// In MainScene.cpp, setup_dialogue_nodes()
void MainScene::setup_dialogue_nodes()
{
    auto &camera = root_.get_child<0>();
    
    // Create dialogue manager
    dialogue_manager_ = std::make_unique<DialogueManager>();
    dialogue_manager_->init(scene_state_);
    
    // Register dialogue textures
    dialogue_manager_->register_texture("intro_1", &intro_1_);
    dialogue_manager_->register_texture("intro_2", &intro_2_);
    dialogue_manager_->register_texture("intro_3", &intro_3_);
    dialogue_manager_->register_texture("intro_4", &intro_4_);
    dialogue_manager_->register_texture("intro_5", &intro_5_);
    dialogue_manager_->register_texture("first_find", &first_find_);
    dialogue_manager_->register_texture("second_find", &second_find_);
    dialogue_manager_->register_texture("third_find", &third_find_);
    
    // Register dialogue nodes
    auto &intro_text_transform = camera.get_child<2>().get_child<1>();
    auto &intro_text_node = intro_text_transform.get_child<0>();
    dialogue_manager_->register_text_node("intro", &intro_text_node);
    
    auto& first_find_transform = camera.get_child<2>().get_child<2>();
    auto& first_find_node = first_find_transform.get_child<0>();
    dialogue_manager_->register_text_node("first_find", &first_find_node);
    
    auto& second_find_transform = camera.get_child<2>().get_child<3>();
    auto& second_find_node = second_find_transform.get_child<0>();
    dialogue_manager_->register_text_node("second_find", &second_find_node);
    
    auto& third_find_transform = camera.get_child<2>().get_child<4>();
    auto& third_find_node = third_find_transform.get_child<0>();
    dialogue_manager_->register_text_node("third_find", &third_find_node);
    
    // Configure dialogue nodes
    configure_dialogue_text_node(intro_text_transform, intro_text_node, nullptr);
    configure_dialogue_text_node(first_find_transform, first_find_node, &first_find_);
    configure_dialogue_text_node(second_find_transform, second_find_node, &second_find_);
    configure_dialogue_text_node(third_find_transform, third_find_node, &third_find_);
    
    // Register dialogue completion callback
    dialogue_manager_->register_on_dialogue_completed_callback(
        [this](DialogueManager::DialogueState state) {
            handle_dialogue_completed(state);
        });
    
    // Show intro dialogue if not completed
    if (!dialogue_completed_) {
        dialogue_manager_->show_dialogue(DialogueManager::DialogueState::INTRO);
    }
}
```

### Phase 3: Extract Game State Management

#### Step 1: Create GameStateManager Class

```cpp
// game_state_manager.hpp
#ifndef GAME_STATE_MANAGER_HPP
#define GAME_STATE_MANAGER_HPP

#include "system/serializer.hpp"
#include <functional>

namespace cge
{

class GameStateManager
{
public:
    enum class GameState {
        INTRO,
        HIDE_AND_SEEK_1,
        HIDE_AND_SEEK_2,
        HIDE_AND_SEEK_3,
        COMPLETED
    };
    
    GameStateManager();
    ~GameStateManager() = default;
    
    void update(double delta);
    
    void transition_to(GameState new_state);
    GameState get_current_state() const;
    
    int get_find_count() const;
    bool is_dialogue_completed() const;
    bool is_game_completed() const;
    
    void register_on_state_changed_callback(std::function<void(GameState, GameState)> callback);
    
    void serialize(Serializer& serializer) const;
    void deserialize(Serializer& serializer);
    
private:
    GameState current_state_ = GameState::INTRO;
    int find_count_ = 0;
    bool dialogue_completed_ = false;
    bool game_completed_ = false;
    
    std::function<void(GameState, GameState)> on_state_changed_;
};

} // namespace cge

#endif // GAME_STATE_MANAGER_HPP
```

```cpp
// game_state_manager.cpp
#include "game_state_manager.hpp"

namespace cge
{

GameStateManager::GameStateManager()
{
}

void GameStateManager::update(double delta)
{
    // Update game state logic
}

void GameStateManager::transition_to(GameState new_state)
{
    if (current_state_ != new_state) {
        GameState old_state = current_state_;
        current_state_ = new_state;
        
        // Update state-dependent variables
        switch (new_state) {
            case GameState::HIDE_AND_SEEK_1:
                find_count_ = 1;
                dialogue_completed_ = true;
                break;
                
            case GameState::HIDE_AND_SEEK_2:
                find_count_ = 2;
                dialogue_completed_ = true;
                break;
                
            case GameState::HIDE_AND_SEEK_3:
                find_count_ = 3;
                dialogue_completed_ = true;
                break;
                
            case GameState::COMPLETED:
                game_completed_ = true;
                break;
                
            default:
                break;
        }
        
        // Notify listeners
        if (on_state_changed_) {
            on_state_changed_(old_state, current_state_);
        }
    }
}

GameStateManager::GameState GameStateManager::get_current_state() const
{
    return current_state_;
}

int GameStateManager::get_find_count() const
{
    return find_count_;
}

bool GameStateManager::is_dialogue_completed() const
{
    return dialogue_completed_;
}

bool GameStateManager::is_game_completed() const
{
    return game_completed_;
}

void GameStateManager::register_on_state_changed_callback(std::function<void(GameState, GameState)> callback)
{
    on_state_changed_ = callback;
}

void GameStateManager::serialize(Serializer& serializer) const
{
    serializer.write("find_count", find_count_);
    serializer.write("dialogue_completed", dialogue_completed_);
    serializer.write("game_completed", game_completed_);
    
    // Serialize the state as an integer
    int state_value = static_cast<int>(current_state_);
    serializer.write("game_state", state_value);
}

void GameStateManager::deserialize(Serializer& serializer)
{
    serializer.read("find_count", find_count_);
    serializer.read("dialogue_completed", dialogue_completed_);
    serializer.read("game_completed", game_completed_);
    
    // Deserialize the state
    int state_value = 0;
    if (serializer.read("game_state", state_value)) {
        current_state_ = static_cast<GameState>(state_value);
    }
}

} // namespace cge
```

#### Step 2: Update MainScene to Use GameStateManager

```cpp
// In MainScene.hpp
private:
    std::unique_ptr<GameStateManager> game_state_manager_;
```

```cpp
// In MainScene.cpp, init()
void MainScene::init(SDLInfo *sdl_info, IoHandler *io_handler)
{
    // ... existing initialization ...
    
    // Create game state manager
    game_state_manager_ = std::make_unique<GameStateManager>();
    
    // Register state change callback
    game_state_manager_->register_on_state_changed_callback(
        [this](GameStateManager::GameState old_state, GameStateManager::GameState new_state) {
            handle_game_state_changed(old_state, new_state);
        });
    
    // ... rest of initialization ...
}
```

```cpp
// In MainScene.cpp, serialize() and deserialize()
void MainScene::serialize(Serializer& serializer) const
{
    // Serialize character positions
    serializer.write("player_x", player_->get_position_x());
    serializer.write("player_y", player_->get_position_y());
    serializer.write("npc_x", blue_witch_->get_position_x());
    serializer.write("npc_y", blue_witch_->get_position_y());
    
    // Serialize audio state
    serializer.write("waiting_to_clap", waiting_to_clap_);
    
    // Serialize game state
    game_state_manager_->serialize(serializer);
}

void MainScene::deserialize(Serializer& serializer)
{
    // First deserialize game state
    game_state_manager_->deserialize(serializer);
    
    // Check if game was completed
    if (game_state_manager_->is_game_completed()) {
        std::cout << "Loading a completed game - starting a new game instead." << std::endl;
        return;
    }
    
    // Deserialize character positions
    float player_x = 0.0f, player_y = 0.0f;
    float npc_x = 0.0f, npc_y = 0.0f;
    
    if (serializer.read("player_x", player_x) && serializer.read("player_y", player_y)) {
        player_->set_position(player_x, player_y);
    }
    
    if (serializer.read("npc_x", npc_x) && serializer.read("npc_y", npc_y)) {
        blue_witch_->set_position(npc_x, npc_y);
    }
    
    // Deserialize audio state
    serializer.read("waiting_to_clap", waiting_to_clap_);
    
    // Update NPC visibility based on game state
    if (game_state_manager_->get_find_count() > 0) {
        blue_witch_->hide();
    }
    
    // Update dialogue state
    if (game_state_manager_->is_dialogue_completed()) {
        dialogue_manager_->hide_all_dialogue();
    }
}
```

## Conclusion

This refactoring guide provides a roadmap for transforming the `MainScene` class into a more maintainable and extensible design. By extracting functionality into dedicated classes and implementing proper design patterns, we can significantly improve the code quality while maintaining the existing functionality.

The refactoring should be approached incrementally, starting with the most critical components (Character, Dialogue, and Game State management) and gradually expanding to other areas. This approach allows for continuous testing and validation throughout the refactoring process.

Once these core refactorings are complete, additional improvements can be made to further enhance the engine's architecture, such as implementing a proper event system, improving scene graph navigation, and developing higher-level scene creation utilities.
