/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef EXAMPLES_MAIN_MENU_HPP
#define EXAMPLES_MAIN_MENU_HPP

#include "platform/scene.hpp"
#include "graph/root_node.hpp"
#include "graph/camera_node.hpp"
#include "graph/sprite_node.hpp"
#include "graph/transform_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/geometry_node.hpp"

namespace cge
{

// Main menu scene for the game
class MainMenuScene : public Scene
{
public:
    MainMenuScene() = default;
    virtual ~MainMenuScene() = default;

    // Scene interface implementation
    virtual void init(SDLInfo* sdl_info, IoHandler* io_handler) override;
    virtual void destroy() override;
    virtual void render() override;
    virtual void update(double delta) override;

    // Serializable overrides
    virtual void serialize(Serializer& serializer) const override;
    virtual void deserialize(Serializer& serializer) override;

    // Called when scene becomes active (top of stack)
    virtual void on_enter() override;

    // Called when scene no longer active (not on top of stack)
    virtual void on_exit() override;

    // Called when scene is paused (covered by another scene)
    virtual void on_pause() override;

    // Called when scene is resumed (uncovered)
    virtual void on_resume() override;

private:
    // Configure graph
    using MenuBackground = TransformNodeT<TextureNodeT<GeometryNodeT<>>>;
    using MenuButton = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
    using MenuGraph = CameraNodeT<MenuBackground, MenuButton, MenuButton>;
    using UIRoot = RootNodeT<MenuGraph>;

    // Set root
    UIRoot root_;

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
    TextureNode new_game_clicked_texture_;

    // Button state tracking
    bool is_new_game_hovered_ = false;
};

} // namespace cge

#endif // EXAMPLES_MAIN_MENU_HPP
