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

// Base class for all characters; inherits from Serializable to serialize
// Character-specific data.
class Character : public Serializable
{
public:
	Character(TransformNode* transform_node, SpriteNode* sprite_node) 
		: transform_node_(transform_node),
		  sprite_node_(sprite_node)
		{}

	~Character() = default;

	virtual void update(double delta);
	virtual void init(SceneState& scene_state) = 0;
	
	// Destroy textures owned by this character
	virtual void destroy_textures() = 0;
	
	// Set a character's position in world space.
	void set_position(float x, float y);

	// Get character's x position in world space.
	float get_position_x() const;

	// Get character's y position in world space.
	float get_position_y() const;

	// Set the character's animation.
	void set_animation(const std::string& animation_name);

	// Set the character's texture. 
	void set_texture(TextureNode* texture);
	
	// Serialize character-specific data. Pure virtual to force 
	// derived classes to implement any necessary serialization 
	// behavior.
	virtual void serialize(Serializer& serializer) const override = 0;

	// Deserialize character-specific data. Pure virtual to force 
	// derived classes to implement any necessary deserialization 
	// behavior.
	virtual void deserialize(Serializer& serializer) override = 0;
	
protected:
	// Helper method to configure a texture with common settings
	void configure_texture(TextureNode& texture, const std::string& filepath, SceneState& scene_state, 
						   bool blend = true, int alpha = 200, int rows = 1, int cols = 1, 
						   int width = 0, int height = 0);

	// Pointer to a character's transform node. Used to handle 
	// world space positioning. 
	TransformNode* transform_node_;

	// Pointer to a character's sprite node. Used to 
	// handle animation and texture updates.
	SpriteNode* sprite_node_;
};

} // namespace cge

#endif // EXAMPLES_CHARACTER_HPP
