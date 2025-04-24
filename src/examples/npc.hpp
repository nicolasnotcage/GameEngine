/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

/*
This header defines a derived NPC class from the Character base class.
The class is intended to demonstrate re-usable classing for NPCs. In 
a real implementation, we could have multiple derived types for various 
NPCs.
*/

#ifndef EXAMPLES_NPC_HPP
#define EXAMPLES_NPC_HPP

#include "examples/character.hpp"
#include <string>

namespace cge
{

// Forward declarations
class SpriteNode;
class TransformNode;
class TextureNode;
class AudioComponent;
struct Path;

// Pause menu scene for the game
class NPC : public Character
{
public:
	NPC(TransformNode* transform_node, SpriteNode* sprite_node)
		: Character(transform_node, sprite_node)
		{}

	void update(double delta) override;
	void init_animations() override;
	void init_audio() override;
	
	// NPC hiding functions
	void hide();
	void show();
	bool is_hidden() const { return hidden_; }

	// Path and movement
	void set_path(Path& path);
	bool is_moving() const;
	void teleport_to(float x, float y);
	bool is_at_position(float y) const;

	// Texture management
	void set_hidden_texture(TextureNode* texture) { hidden_texture_ = texture; }
	void set_idle_texture(TextureNode* texture) { idle_texture_ = texture; }

	// Audio handling
	void process_audio_timing(double delta);
	void clap_if_hidden();
	void set_waiting_to_clap(bool waiting) { waiting_to_clap_ = waiting; }
	bool is_waiting_to_clap() const { return waiting_to_clap_; }
	void reset_audio_timer() { audio_timer_ = 0.0f; }

	// Dialogue state
	void start_waiting_for_dialogue() { waiting_for_dialogue_ = true; }
	void stop_waiting_for_dialogue() { waiting_for_dialogue_ = false; }
	bool is_waiting_for_dialogue() const { return waiting_for_dialogue_; }

private:
	bool hidden_{false};
	bool waiting_for_dialogue_{false};
	bool waiting_to_clap_{false};
	float audio_timer_{0.0f};
	float time_to_clap_{1.0f};
	TextureNode* hidden_texture_{nullptr};
	TextureNode* idle_texture_{nullptr};
};

} // namespace cge

#endif // EXAMPLES_NPC_HPP
