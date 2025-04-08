/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GRAPH_UI_BUTTON_HPP
#define GRAPH_UI_BUTTON_HPP

#include "graph/node.hpp"
#include "graph/sprite_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/transform_node.hpp"
#include "platform/io_handler.hpp"

#include <functional>
#include <string>

namespace cge
{

// Forward declarations
class CameraNode;

// A UI button component that can be clicked
class UIButton : public Node
{
public:
    UIButton();
    virtual ~UIButton() = default;

    // Node interface implementation
    virtual void init(SceneState &scene_state) override;
    virtual void destroy() override;
    virtual void draw(SceneState &scene_state) override;
    virtual void update(SceneState &scene_state) override;

    // Button specific methods
    void set_position(float x, float y);
    void set_size(float width, float height);
    void set_normal_sprite(const std::string& filepath);
    void set_hover_sprite(const std::string& filepath);
    void set_pressed_sprite(const std::string& filepath);
    void set_callback(std::function<void()> callback);
    
    // Set associated nodes
    void set_transform_node(TransformNode* transform);
    void set_sprite_node(SpriteNode* sprite);
    void set_camera_node(CameraNode* camera);
    
    // Check if a point is inside the button
    bool contains_point(float x, float y) const;

private:
    enum class ButtonState
    {
        NORMAL,
        HOVER,
        PRESSED
    };

    ButtonState state_;
    std::function<void()> callback_;
    
    // Button position and size
    float x_;
    float y_;
    float width_;
    float height_;
    
    // Sprite nodes for different button states
    TextureNode normal_texture_;
    TextureNode hover_texture_;
    TextureNode pressed_texture_;
    
    // Current active texture
    TextureNode* current_texture_;
    
    // References to nodes in the graph
    TransformNode* transform_node_;
    SpriteNode* sprite_node_;
    CameraNode* camera_node_;

    void display_init_error();
};

template <typename... ChildrenTs>
using UIButtonT = NodeT<UIButton, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_UI_BUTTON_HPP
