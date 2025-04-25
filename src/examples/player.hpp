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
	void init(SceneState& scene_state) override;
	void init_animations();
	void init_audio();
	void init_textures(SceneState& scene_state);
	void destroy_textures() override;
	
	// Audio actions
	void whistle();
	void process_audio_actions(IoHandler* io_handler, std::shared_ptr<NPC>);
	
	// Input handling
	void handle_input_actions(IoHandler* io_handler);
	bool is_investigating(IoHandler* io_handler) const;
	
	// Serializable interface implementation
	void serialize(Serializer& serializer) const override;
	void deserialize(Serializer& serializer) override;
	
private:
	// Owned textures
	TextureNode run_texture_;
	TextureNode idle_texture_;
};

} // namespace cge

#endif // EXAMPLES_PLAYER_HPP
