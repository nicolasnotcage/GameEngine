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

#include <string>

namespace cge
{

// Forward declarations
class SpriteNode;
class TextureNode;
class TransformNode;

// Pause menu scene for the game
class Character
{
public:
	Character(TransformNode* transform_node, SpriteNode* sprite_node) 
		: transform_node_(transform_node),
		  sprite_node_(sprite_node)
		{}

	~Character() = default;

	virtual void update(double delta);
	void set_position(float x, float y);
	float get_position_x() const;
	float get_position_y() const;

	void set_animation(const std::string& animation_name);
	void set_texture(TextureNode* texture);

	TransformNode* get_transform_node() { return transform_node_; }
	
protected:
	TransformNode* transform_node_;
	SpriteNode* sprite_node_;
};

} // namespace cge

#endif // EXAMPLES_CHARACTER_HPP
