/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef EXAMPLES_PAUSE_MENU_HPP
#define EXAMPLES_PAUSE_MENU_HPP

#include "examples/ui_scene.hpp"
#include "graph/texture_node.hpp"

namespace cge
{

// Pause menu scene for the game
class PauseMenuScene : public UIScene
{
public:
    PauseMenuScene() = default;
    virtual ~PauseMenuScene() = default;

    // Scene interface implementation
    virtual void init(SDLInfo* sdl_info, IoHandler* io_handler) override;
    virtual void destroy() override;
    virtual void render() override;
    virtual void update(double delta) override;
    
    // Pause menu is a full-screen menu (non-transparent)
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
    void on_resume_clicked();

    // Textures
    TextureNode background_texture_;
    TextureNode resume_texture_;
    TextureNode resume_hover_texture_;

    // Button state tracking
    bool is_resume_hovered_ = false;
};

} // namespace cge

#endif // EXAMPLES_PAUSE_MENU_HPP
