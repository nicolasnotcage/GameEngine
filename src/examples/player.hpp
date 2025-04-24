/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

/*
This header defines a derived Player class from the base Character class. 
There should likely only be one instance of a Player class in a game, as it
represented the user.
*/

#ifndef EXAMPLES_PLAYER_HPP
#define EXAMPLES_PLAYER_HPP

#include "examples/character.hpp"

#include <string>

namespace cge
{

// Forward declarations
class SpriteNode;
class TextureNode;
class TransformNode;
class IoHandler;
class NPC;

// Pause menu scene for the game
class Player : public Character
{
public:
	Player(TransformNode* transform_node, SpriteNode* sprite_node) 
		: Character(transform_node, sprite_node)
		{}

	void update(double delta) override;
	void init_animations() override;
	void init_audio() override;
	
	// Audio actions
	void whistle();
	void process_audio_actions(IoHandler* io_handler, std::shared_ptr<NPC>);
	
	// Input handling
	void handle_input_actions(IoHandler* io_handler);
	bool is_investigating(IoHandler* io_handler) const;
	
	// Texture management
	void set_run_texture(TextureNode* texture) { run_texture_ = texture; }
	void set_idle_texture(TextureNode* texture) { idle_texture_ = texture; }
	
private:
	TextureNode* run_texture_{nullptr};
	TextureNode* idle_texture_{nullptr};
};

} // namespace cge

#endif // EXAMPLES_PLAYER_HPP
