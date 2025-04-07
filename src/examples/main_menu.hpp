/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef EXAMPLES_MAIN_MENU_HPP
#define EXAMPLES_MAIN_MENU_HPP

#include "examples/ui_scene.hpp"
#include "graph/texture_node.hpp"

namespace cge
{

// Main menu scene for the game
class MainMenuScene : public UIScene
{
public:
    MainMenuScene() = default;
    virtual ~MainMenuScene() = default;

    // Scene interface implementation
    virtual void init(SDLInfo* sdl_info, IoHandler* io_handler) override;
    virtual void destroy() override;
    virtual void render() override;
    virtual void update(double delta) override;
    
    // Main menu is not transparent (it's a full-screen menu)
    virtual bool is_transparent() const override { return false; }

    // Serializable overrides
    virtual void serialize(Serializer& serializer) const override;
    virtual void deserialize(Serializer& serializer) override;

private:
    // Setup UI elements
    void setup_background();
    void setup_buttons();

    // Button interaction and callback methods
    void check_button_interactions();
    void on_new_game_clicked();

    // Textures
    TextureNode background_texture_;
    TextureNode new_game_texture_;
    TextureNode new_game_hover_texture_;

    // Button state tracking
    bool is_new_game_hovered_ = false;
};

} // namespace cge

#endif // EXAMPLES_MAIN_MENU_HPP
