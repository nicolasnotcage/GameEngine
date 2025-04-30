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
#include "system/serializable.hpp"
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>

namespace cge
{

class DialogueManager : public Serializable
{
public:
    // Scene-specifc enum for each stage of the mains scene
    enum class DialogueState 
    {
        INACTIVE,
        INTRO,
        FIRST_FIND,
        SECOND_FIND,
        THIRD_FIND,
        COMPLETED
    };
    
    // Helper function to convert DialogueState to string
    static const char* dialogue_state_to_string(DialogueState state);
    
    // Friend function to allow std::cout << DialogueState
    friend std::ostream& operator<<(std::ostream& os, const DialogueState& state);
    
    DialogueManager();
    ~DialogueManager() = default;
    
    // Initialize the dialogue manager with scene state. Requires
    // that SceneState be passed because we must initialize the 
    // textures owned by the class.
    void init(SceneState& scene_state);

    // Check if any dialogue is active or if dialogue should be 
    // deactivated and callbacks called. 
    void update();
    
    // TextNode objects are registered in DialogueEntry structs with pointers to the TextNode
    // and an associated ID. Entries are defaulted to inactive. 
    void register_text_node(const std::string& id, TextNode* node);

    // Textures are registered by passing an associated ID and filepath. They are stored in 
    // a TextureEntry struct and pushed to a vector of TextureEntries. Texture objects are 
    // also stored within the TextureEntry struct but aren't initialized until the dedicated
    // init function is called. 
    void register_texture(const std::string& id, const std::string& filepath);

    // Initialize the textures stored within the vector of TextureEntries.
    void init_textures(SceneState& scene_state);

    // Destory the texture objects stored within the vector of TextureEntries.
    void destroy_textures();
    
    // Render the dialogue associated with the given DialogueState.
    void show_dialogue(DialogueState state);

    // Stops rendering all dialogue, 
    void hide_all_dialogue();
    
    // Return if dialogue is active. 
    bool is_dialogue_active() const;

    // Return the current DialogueState.
    DialogueState get_current_state() const;
    
    // Register a callback function to be executed upon dialogue completion.
    void register_on_dialogue_completed_callback(std::function<void(DialogueState)> callback);
    
    // Serialize dialogue-specific data. 
    void serialize(Serializer& serializer) const override;

    // Deserialize dialogue-specific data. 
    void deserialize(Serializer& serializer) override;
    
private:
    // Utility struct containing (1) an identifier, (2) a TextNode poiner, 
    // and (3) an active flag. The struct is primarily used to manage the
    // dialogue entities contained within the scene, helping to facilitate 
    // smooth dialogue flow. 
    struct DialogueEntry 
    {
        std::string id;
        TextNode* node;
        bool active = false;
    };
    
    // A vector of dialogue entries. 
    std::vector<DialogueEntry> dialogue_entries_;

    // Texture storage
    struct TextureEntry 
    {
        std::string id;
        std::string filepath;
        TextureNode texture;
    };
    
    // A vector of texture entries.
    std::vector<TextureEntry> textures_;
    
    // The current state of the scene's dialogue. Initialized 
    // to inactive.
    DialogueState current_state_{DialogueState::INACTIVE};

    // Flag indicating whether dialogue is actively being
    // rendered.
    bool dialogue_active_ = false;
    
    // A callback function that is executed when dialogue is completed. 
    std::function<void(DialogueState)> on_dialogue_completed_;
};

} // namespace cge

#endif // EXAMPLES_DIALOGUE_MANAGER_HPP
