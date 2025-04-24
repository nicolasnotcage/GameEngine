/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/character.hpp"
#include "graph/transform_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/sprite_node.hpp"
#include "system/serializer.hpp"

namespace cge
{

void Character::update(double delta)
{
		// Main update logic
}

// Getters
float Character::get_position_x() const { return transform_node_->get_position_x(); }
float Character::get_position_y() const { return transform_node_->get_position_y(); }

// Setters
void Character::set_position(float x, float y) { transform_node_->set_position(x, y); }
void Character::set_animation(const std::string& animation_name) { sprite_node_->play(animation_name); }
void Character::set_texture(TextureNode* texture_node) { sprite_node_->set_texture(texture_node); }

// Helper method to configure a texture with common settings
void Character::configure_texture(TextureNode& texture, const std::string& filepath, SceneState& scene_state,
                                 bool blend, int alpha, int rows, int cols, int width, int height)
{
    texture.set_filepath(filepath);
    texture.set_blend(blend);
    texture.set_blend_alpha(alpha);
    
    if (cols > 1 || rows > 1) {
        texture.define_grid(rows, cols, width, height);
    }

    texture.init(scene_state);
}

// Serialization is now implemented in derived classes

} // namespace cge
