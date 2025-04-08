/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef EXAMPLES_MAIN_MENU_HPP
#define EXAMPLES_MAIN_MENU_HPP

#include "graph/root_node.hpp"
#include "graph/camera_node.hpp"
#include "graph/sprite_node.hpp"
#include "graph/transform_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/geometry_node.hpp"

#include "platform/audio_component.hpp"
#include "platform/io_handler.hpp"
#include "platform/path.hpp"
#include "platform/scene.hpp"

#include <array>


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

private:
    // Configure graph
    using MenuBackground = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
    using MenuButton = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
    using MenuTitle = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
    using MenuGraph = CameraNodeT<MenuBackground, MenuTitle, MenuButton, MenuButton, MenuButton, MenuButton>;
    using UIRoot = RootNodeT<MenuGraph>;

    // Set root
    UIRoot root_;

    SDLInfo* sdl_info_;
    SceneState scene_state_;
    IoHandler* io_handler_;

    // Setup functions
    void initialize_textures();

    // Utility functions
    void update_button_stats();
    void update_hovering(Vector2 mouse_position);

    // Textures
    TextureNode background_texture_;
    TextureNode title_texture_;
    
    // New Game button textures
    TextureNode new_game_texture_;
    TextureNode new_game_hover_texture_;
    TextureNode new_game_clicked_texture_;
    
    // Load Game button textures
    TextureNode load_game_texture_;
    TextureNode load_game_hover_texture_;
    TextureNode load_game_clicked_texture_;
    
    // Settings button textures
    TextureNode settings_texture_;
    TextureNode settings_hover_texture_;
    TextureNode settings_clicked_texture_;
    
    // Exit button textures
    TextureNode exit_texture_;
    TextureNode exit_hover_texture_;
    TextureNode exit_clicked_texture_;

    // Button state tracking
    bool is_new_game_hovered_ = false;
    bool is_load_game_hovered_ = false;
    bool is_settings_hovered_ = false;
    bool is_exit_hovered_ = false;

    // Button info
    std::array<float, 2> new_game_button_position;
    std::array<float, 2> new_game_button_scale;
    std::array<float, 2> load_game_button_position;
    std::array<float, 2> load_game_button_scale;
    std::array<float, 2> settings_button_position;
    std::array<float, 2> settings_button_scale;
    std::array<float, 2> exit_button_position;
    std::array<float, 2> exit_button_scale;
};

} // namespace cge

#endif // EXAMPLES_MAIN_MENU_HPP
