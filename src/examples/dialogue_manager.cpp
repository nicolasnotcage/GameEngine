/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/dialogue_manager.hpp"
#include "system/serializer.hpp"

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

void DialogueManager::register_texture(const std::string& id, const std::string& filepath)
{
    TextureEntry entry;
    entry.id = id;
    entry.filepath = filepath;
    textures_.push_back(entry);
}

void DialogueManager::init_textures(SceneState& scene_state)
{
    for (auto& entry : textures_) {
        entry.texture.set_filepath(entry.filepath);
        entry.texture.set_blend(true);
        entry.texture.set_blend_alpha(200);
        entry.texture.init(scene_state);
    }
}

void DialogueManager::destroy_textures()
{
    for (auto& entry : textures_) {
        entry.texture.destroy();
    }
}

void DialogueManager::show_dialogue(DialogueState state)
{
    current_state_ = state;
    dialogue_active_ = true;
    
    // Hide all dialogues first
    hide_all_dialogue();
    
    // Find the appropriate dialogue node and texture
    TextNode* node_to_show = nullptr;
    std::string texture_id;
    
    switch (state) {
        case DialogueState::INTRO:
            // Find intro dialogue node
            for (auto& entry : dialogue_entries_) {
                if (entry.id == "intro") {
                    entry.active = true;
                    node_to_show = entry.node;
                    break;
                }
            }
            
            // Set intro textures
            if (node_to_show) {
                node_to_show->clear_textures();
                
                // Add all intro textures
                for (int i = 1; i <= 5; i++) {
                    std::string id = "intro_" + std::to_string(i);
                    for (auto& tex_entry : textures_) {
                        if (tex_entry.id == id) {
                            node_to_show->push_texture(&tex_entry.texture);
                        }
                    }
                }
                
                node_to_show->set_should_render(true);
            }
            break;
            
        case DialogueState::FIRST_FIND:
            texture_id = "first_find";
            for (auto& entry : dialogue_entries_) {
                if (entry.id == "first_find") {
                    entry.active = true;
                    node_to_show = entry.node;
                    break;
                }
            }
            break;
            
        case DialogueState::SECOND_FIND:
            texture_id = "second_find";
            for (auto& entry : dialogue_entries_) {
                if (entry.id == "second_find") {
                    entry.active = true;
                    node_to_show = entry.node;
                    break;
                }
            }
            break;
            
        case DialogueState::THIRD_FIND:
            texture_id = "third_find";
            for (auto& entry : dialogue_entries_) {
                if (entry.id == "third_find") {
                    entry.active = true;
                    node_to_show = entry.node;
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
    
    // Set texture for find dialogues
    if (node_to_show && !texture_id.empty()) {
        node_to_show->clear_textures();
        
        for (auto& tex_entry : textures_) {
            if (tex_entry.id == texture_id) {
                node_to_show->push_texture(&tex_entry.texture);
                node_to_show->set_should_render(true);
                break;
            }
        }
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

// Serializable interface implementation
void DialogueManager::serialize(Serializer& serializer) const
{
    // Serialize dialogue state
    int state = static_cast<int>(current_state_);
    serializer.write("dialogue_state", state);
    serializer.write("dialogue_active", dialogue_active_);
    serializer.write("dialogue_completed", current_state_ == DialogueState::COMPLETED);
}

void DialogueManager::deserialize(Serializer& serializer)
{
    // Deserialize dialogue state
    int state = 0;
    if (serializer.read("dialogue_state", state)) {
        current_state_ = static_cast<DialogueState>(state);
    }
    
    serializer.read("dialogue_active", dialogue_active_);
    
    // Check if dialogue was completed
    bool dialogue_completed = false;
    if (serializer.read("dialogue_completed", dialogue_completed) && dialogue_completed) {
        current_state_ = DialogueState::COMPLETED;
    }
    
    // Update dialogue visibility based on state
    if (dialogue_active_ && current_state_ != DialogueState::INACTIVE) {
        show_dialogue(current_state_);
    } else {
        hide_all_dialogue();
    }
}

} // namespace cge
