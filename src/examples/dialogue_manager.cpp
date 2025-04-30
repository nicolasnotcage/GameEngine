/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/dialogue_manager.hpp"
#include "system/serializer.hpp"

#include <iostream>

namespace cge
{

// Helper function to convert DialogueState to string
const char* DialogueManager::dialogue_state_to_string(DialogueState state)
{
    switch (state)
    {
        case DialogueState::INACTIVE: return "INACTIVE";
        case DialogueState::INTRO: return "INTRO";
        case DialogueState::FIRST_FIND: return "FIRST_FIND";
        case DialogueState::SECOND_FIND: return "SECOND_FIND";
        case DialogueState::THIRD_FIND: return "THIRD_FIND";
        case DialogueState::COMPLETED: return "COMPLETED";
        default: return "UNKNOWN";
    }
}

// Friend function to allow std::cout << DialogueState
std::ostream& operator<<(std::ostream& os, const DialogueManager::DialogueState& state)
{
    os << DialogueManager::dialogue_state_to_string(state);
    return os;
}

DialogueManager::DialogueManager() {}

// Initialize the textures owned by the DialogueManager.
void DialogueManager::init(SceneState& scene_state)
{
    init_textures(scene_state);
}

// Check if any active dialogue has been dismissed
void DialogueManager::update()
{
    bool any_active = false;
    
    for (auto& entry : dialogue_entries_) 
    {
        if (entry.active && entry.node) 
        {
            // If the TextNode is not being rendered, update its active
            // flag to false and trigger callback.
            if (!entry.node->is_rendered()) 
            {
                entry.active = false;
                
                // Dialogue was dismissed, trigger callback
                if (on_dialogue_completed_ && !any_active) 
                {
                    on_dialogue_completed_(current_state_);
                    dialogue_active_ = false;
                }
            } 
            // Entry is being rendered, so we do have an active instance
            // of dialogue.
            else 
            {
                any_active = true;
            }
        }
    }
    
    dialogue_active_ = any_active;
}

// TextNode objects are registered in DialogueEntry structs with pointers to the TextNode
// and an associated ID. Entries are defaulted to inactive. 
void DialogueManager::register_text_node(const std::string& id, TextNode* node)
{
    if (node) 
    {
        DialogueEntry entry;
        entry.id = id;
        entry.node = node;
        entry.active = false;
        dialogue_entries_.push_back(entry);
    }
    else
    {
        std::cerr << "Error registering TextNode in DialogueManager with ID = " << id << "\n";
    }
}

// Textures are registered by passing an associated ID and filepath. They are stored in 
// a TextureEntry struct and pushed to a vector of TextureEntries. Texture objects are 
// also stored within the TextureEntry struct but aren't initialized until the dedicated
//  init function is called. 
void DialogueManager::register_texture(const std::string& id, const std::string& filepath)
{
    TextureEntry entry;
    entry.id = id;
    entry.filepath = filepath;
    textures_.push_back(entry);
}

// Initialize the textures stored within the vector of TextureEntries.
void DialogueManager::init_textures(SceneState& scene_state)
{
    for (auto& entry : textures_) 
    {
        entry.texture.set_filepath(entry.filepath);
        entry.texture.set_blend(true);
        entry.texture.set_blend_alpha(200);
        entry.texture.init(scene_state);
    }
}

// Destory the texture objects stored within the vector of TextureEntries.
void DialogueManager::destroy_textures()
{
    for (auto& entry : textures_) 
    {
        entry.texture.destroy();
    }
}

// Render the dialogue associated with the given state. 
void DialogueManager::show_dialogue(DialogueState state)
{
    current_state_ = state;
    dialogue_active_ = true;
    
    // Hide all dialogues first
    hide_all_dialogue();
    
    // Find the appropriate dialogue node and texture
    TextNode* node_to_show = nullptr;
    std::string texture_id;
    
    switch (state) 
    {
        case DialogueState::INTRO:
            // Find intro dialogue node
            for (auto& entry : dialogue_entries_) 
            {
                if (entry.id == "intro") 
                {
                    entry.active = true;
                    node_to_show = entry.node;
                    break;
                }
            }
            
            // Set intro textures. Note that Intro dialogue handles its own TextNode configuration, whereas
            // the remaining cases are handled in the general case after the switch statement. 
            if (node_to_show) 
            {
                // Clear any existing textures stored within the node
                node_to_show->clear_textures();
                
                // Iteratively add all intro textures to the TextNode. Intro textures
                // are defined in main_scene as intro_1, intro_2, etc. so we just match
                // that pattern. This is admittedly heavily dependent on data specified 
                // in the main scene and could be simplified. 
                for (int i = 1; i <= 5; i++) 
                {
                    std::string id = "intro_" + std::to_string(i);
                    for (auto& tex_entry : textures_) 
                    {
                        // Push intro textures with matching ID to the TextNode
                        if (tex_entry.id == id) 
                        {
                            node_to_show->push_texture(&tex_entry.texture);
                        }
                    }
                }
                
                node_to_show->set_should_render(true);
            }
            break;
            
        case DialogueState::FIRST_FIND:
            // Find "first find" dialogue node
            texture_id = "first_find";
            for (auto& entry : dialogue_entries_) 
            {
                if (entry.id == "first_find") 
                {
                    entry.active = true;
                    node_to_show = entry.node;
                    break;
                }
            }
            break;
            
        case DialogueState::SECOND_FIND:
            // Find "second find" dialogue node
            texture_id = "second_find";
            for (auto& entry : dialogue_entries_) 
            {
                if (entry.id == "second_find") 
                {
                    entry.active = true;
                    node_to_show = entry.node;
                    break;
                }
            }
            break;
            
        case DialogueState::THIRD_FIND:
            // Find "third find" dialogue node
            texture_id = "third_find";
            for (auto& entry : dialogue_entries_) 
            {
                if (entry.id == "third_find") 
                {
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
            std::cerr << "DialogueManager::show_dialogue() called with unexpected dialogue state.\n";
            break;
    }
    
    // Set texture for find dialogues
    if (node_to_show && !texture_id.empty()) 
    {
        node_to_show->clear_textures();
        
        for (auto& tex_entry : textures_) 
        {
            if (tex_entry.id == texture_id) 
            {
                node_to_show->push_texture(&tex_entry.texture);
                node_to_show->set_should_render(true);
                break;
            }
        }
    }
}

// Hide all dialogue in the scene. 
void DialogueManager::hide_all_dialogue()
{
    for (auto& entry : dialogue_entries_) 
    {
        entry.active = false;
        if (entry.node) entry.node->set_should_render(false);
    }
}

// Return if dialogue is currently active.
bool DialogueManager::is_dialogue_active() const
{
    return dialogue_active_;
}

// Return current dialogue state.
DialogueManager::DialogueState DialogueManager::get_current_state() const
{
    return current_state_;
}

// Register a callback to be executed when dialogue is completed. 
void DialogueManager::register_on_dialogue_completed_callback(std::function<void(DialogueState)> callback)
{
    on_dialogue_completed_ = callback;
}

// Serialize dialogue state.
void DialogueManager::serialize(Serializer& serializer) const
{
    int state = static_cast<int>(current_state_);
    serializer.write("dialogue_state", state);
    serializer.write("dialogue_active", dialogue_active_);
}

// Deserialize dialogue state.
void DialogueManager::deserialize(Serializer& serializer)
{
    int state = 0;
    if (serializer.read("dialogue_state", state)) current_state_ = static_cast<DialogueState>(state);
    serializer.read("dialogue_active", dialogue_active_);

    // Update dialogue visibility based on state
    if (dialogue_active_ && current_state_ != DialogueState::INACTIVE) show_dialogue(current_state_);
    else hide_all_dialogue();
}

} // namespace cge
