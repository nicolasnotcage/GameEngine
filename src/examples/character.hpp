/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

/*
This header defines a base Character class. Derived classes are used to abstract
character-based behavior in scenes. For example, a Player class can derive from 
Character and implement player-specific controllers and functionality. NPC classes 
can implement NPC-specific behavior, allowing for re-usable NPC types.
*/

#ifndef EXAMPLES_CHARACTER_HPP
#define EXAMPLES_CHARACTER_HPP

#include "graph/texture_node.hpp"
#include "graph/scene_state.hpp"
#include "system/serializable.hpp"
#include <string>
#include <memory>

namespace cge
{

// Forward declarations
class SpriteNode;
class TransformNode;

// Base class for all characters
class Character : public Serializable
{
public:
	Character(TransformNode* transform_node, SpriteNode* sprite_node) 
		: transform_node_(transform_node),
		  sprite_node_(sprite_node)
		{}

	~Character() = default;

	virtual void update(double delta);
	virtual void init_animations() = 0; // Pure virtual function
	virtual void init_audio() = 0; // Pure virtual function for audio setup
	virtual void init_textures(SceneState& scene_state) = 0; // Pure virtual function for texture loading
	
	// Destroy textures owned by this character
	virtual void destroy_textures() = 0;
	
	void set_position(float x, float y);
	float get_position_x() const;
	float get_position_y() const;

	void set_animation(const std::string& animation_name);
	void set_texture(TextureNode* texture);

	TransformNode* get_transform_node() { return transform_node_; }
	
	// Serializable interface implementation - pure virtual
	virtual void serialize(Serializer& serializer) const override = 0;
	virtual void deserialize(Serializer& serializer) override = 0;
	
protected:
	// Helper method to configure a texture with common settings
	void configure_texture(TextureNode& texture, const std::string& filepath, SceneState& scene_state, 
						   bool blend = true, int alpha = 200, int rows = 1, int cols = 1, 
						   int width = 0, int height = 0);

	TransformNode* transform_node_;
	SpriteNode* sprite_node_;
};

} // namespace cge

#endif // EXAMPLES_CHARACTER_HPP
