/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef EXAMPLES_UI_SCENE_HPP
#define EXAMPLES_UI_SCENE_HPP

#include "platform/scene.hpp"
#include "graph/ui_button.hpp"
#include "graph/root_node.hpp"
#include "graph/camera_node.hpp"
#include "graph/scene_state.hpp"
#include "graph/geometry_node.hpp"
#include "graph/sprite_node.hpp"
#include "graph/texture_node.hpp"
#include "system/serializer.hpp"

#include <vector>
#include <memory>

namespace cge
{

// Base class for UI scenes like menus
class UIScene : public Scene
{
public:
    UIScene() = default;
    virtual ~UIScene() = default;

    // Scene interface implementation
    virtual void init(SDLInfo* sdl_info, IoHandler* io_handler) override;
    virtual void destroy() override;
    virtual void render() override;
    virtual void update(double delta) override;

    // Serializable overrides
    virtual void serialize(Serializer& serializer) const override;
    virtual void deserialize(Serializer& serializer) override;

    // Returns whether the scene is transparent (defaults to false)
    virtual bool is_transparent() const { return false; }

protected:
    // Add a button to the UI
    UIButton* add_button(float x, float y, float width, float height, 
                        const std::string& normal_sprite,
                        const std::string& hover_sprite = "",
                        const std::string& pressed_sprite = "");

    // Set a callback for a button
    void set_button_callback(UIButton* button, std::function<void()> callback);

    // Scene state and rendering
    SDLInfo* sdl_info_ = nullptr;
    IoHandler* io_handler_ = nullptr;
    SceneState scene_state_;

    // Define menu button and background nodes
    using MenuButton = TransformNodeT<SpriteNodeT<GeometryNodeT<>>>;
    using MenuBackground = TransformNodeT<TextureNodeT<GeometryNodeT<>>>;
    
    // Define menu scene graph with background and buttons
    using MenuGraph = CameraNodeT<MenuBackground, MenuButton, MenuButton>;
    
    // Root node for the UI scene graph
    using UIRoot = RootNodeT<MenuGraph>;
    UIRoot root_;

    // Store buttons for easy access
    std::vector<std::shared_ptr<UIButton>> buttons_;
};

} // namespace cge

#endif // EXAMPLES_UI_SCENE_HPP
