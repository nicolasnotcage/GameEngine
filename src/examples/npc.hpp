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
	void init(SceneState& scene_state) override;
	void init_animations();
	void init_audio();
	void init_textures(SceneState& scene_state);
	void destroy_textures() override;
	
	// NPC hiding functions
	void hide();
	void show();
	bool is_hidden() const { return hidden_; }

	// Path and movement
	void set_path(Path& path);
	bool is_moving() const;
	void teleport_to(float x, float y);
	bool is_at_position(float y) const;

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
	
	// Serializable interface implementation
	void serialize(Serializer& serializer) const override;
	void deserialize(Serializer& serializer) override;

private:
	bool hidden_{false};
	bool waiting_for_dialogue_{false};
	bool waiting_to_clap_{false};
	float audio_timer_{0.0f};
	float time_to_clap_{1.0f};
	
	// Owned textures
	TextureNode hidden_texture_;
	TextureNode idle_texture_;
	TextureNode run_texture_;
};

} // namespace cge

#endif // EXAMPLES_NPC_HPP
