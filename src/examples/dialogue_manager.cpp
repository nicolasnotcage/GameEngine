/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/dialogue_manager.hpp"

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
            
        case DialogueState::SECOND_FIND:
            for (auto& entry : dialogue_entries_) {
                if (entry.id == "second_find") {
                    entry.active = true;
                    entry.node->set_should_render(true);
                    break;
                }
            }
            break;
            
        case DialogueState::THIRD_FIND:
            for (auto& entry : dialogue_entries_) {
                if (entry.id == "third_find") {
                    entry.active = true;
                    entry.node->set_should_render(true);
                    break;
                }
            }
            break;
            
        case DialogueState::COMPLETED:
            // No dialogue to show for completed state
            break;
            
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
