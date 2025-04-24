/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

/*
This header defines a DialogueManager class that abstracts dialogue handling
from the MainScene class. It manages dialogue states, text nodes, and textures.
*/

#ifndef EXAMPLES_DIALOGUE_MANAGER_HPP
#define EXAMPLES_DIALOGUE_MANAGER_HPP

#include "graph/text_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/scene_state.hpp"
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>

namespace cge
{

class DialogueManager
{
public:
    enum class DialogueState 
    {
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
    struct DialogueEntry 
    {
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

#endif // EXAMPLES_DIALOGUE_MANAGER_HPP
