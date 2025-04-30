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

class Player : public Character
{
public:
	Player(TransformNode* transform_node, SpriteNode* sprite_node) 
		: Character(transform_node, sprite_node)
		{}

	// Update player-specific behavior
	void update(double delta) override;

	// Initialize player-specific data
	void init(SceneState& scene_state) override;

	// Initialize player-specific animations
	void init_animations();

	// Initialize player-specific audio
	void init_audio();

	// Initialize player-specific textures
	void init_textures(SceneState& scene_state);

	// Destory player-specific textures
	void destroy_textures() override;
	
	// Causes the player to whistle
	void whistle();

	// Identify and handle player-specific audio behaviors. Takes a shared pointer
	// to an NPC as an argument, which is notified if the player whistles. If the 
	// player whistles, then the NPC should clap in response. Could be simplified
	// with a data-oriented GameManager that objects query for updates. 
	void process_audio_actions(IoHandler* io_handler, std::shared_ptr<NPC>);
	
	// Identify and handle player-specific inputs
	void handle_input_actions(IoHandler* io_handler);

	// Return whether or not the player is investigating an area.
	bool is_investigating(IoHandler* io_handler) const;
	
	// Serialize player-specific data
	void serialize(Serializer& serializer) const override;

	// Deserialize player-specific data
	void deserialize(Serializer& serializer) override;
	
private:
	TextureNode run_texture_;	// Texture used when player is running
	TextureNode idle_texture_;	// Texture used when player is idle
};

} // namespace cge

#endif // EXAMPLES_PLAYER_HPP
