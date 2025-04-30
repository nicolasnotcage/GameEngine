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

class NPC : public Character
{
public:
	NPC(TransformNode* transform_node, SpriteNode* sprite_node)
		: Character(transform_node, sprite_node)
		{}

	// Update NPC-specific behavior.
	void update(double delta) override;

	// Initialize NPC-specific data.
	void init(SceneState& scene_state) override;

	// Initialize NPC animations.
	void init_animations();

	// Initialize NPC-specific audio.
	void init_audio();

	// Initialize NPC-specific textures.
	void init_textures(SceneState& scene_state);

	// Destroy textures owned by NPCs.
	void destroy_textures() override;
	
	// Hide the NPC.
	void hide();

	// Reveal the hidden NPC.
	void show();

	// Return whether or not the NPC is hidden.
	bool is_hidden() const { return hidden_; }

	// Set the Path to be followed by the NPC.
	void set_path(Path& path);

	// Return whether or not the NPC is currently moving.
	bool is_moving() const;

	// Teleport the NPC to the specified location in world space.
	void teleport_to(float x, float y);

	// Return whether the NPC is located at the world space y position. This 
	// is a utility function used in the main scene to detect when the initial
	// pathing is completed.
	bool is_at_position(float y) const;

	// Facilitates timing-dependent NPC audio responses. 
	void process_audio_timing(double delta);

	// The NPC will play a clapping sound if it is currently hiding.
	void clap_if_hidden();

	// Flag indicating whether the NPC is waiting to clap. Used when processing
	// audio timing.
	void set_waiting_to_clap(bool waiting) { waiting_to_clap_ = waiting; }

	// Return whether or not the NPC is waiting to clap.
	bool is_waiting_to_clap() const { return waiting_to_clap_; }

	// Resets audio timer for next instance of time-dependent audio.
	void reset_audio_timer() { audio_timer_ = 0.0f; }

	// Get the audio component owned by the NPC.
	AudioComponent* get_audio_component();

	// Set the waiting for dialogue flag to true. Facilitates scene state transitions.
	void start_waiting_for_dialogue() { waiting_for_dialogue_ = true; }

	// Set the waiting for dialogue flag to false. Facilitates scene state transitions.
	void stop_waiting_for_dialogue() { waiting_for_dialogue_ = false; }

	// Return whether or not the NPC is waiting for dialogue.
	bool is_waiting_for_dialogue() const { return waiting_for_dialogue_; }
	
	// Serialize NPC-specific data.
	void serialize(Serializer& serializer) const override;

	// Deserialize NPC-specific data.
	void deserialize(Serializer& serializer) override;

private:
	// NPC data
	bool hidden_{false};				// Whether or not the NPC is currently hidden.
	bool waiting_for_dialogue_{false};	// Whether or not the NPC is currently waiting for dialogue to continue behavior.
	bool waiting_to_clap_{false};		// Whether or not the NPC is waiting to clap.
	float audio_timer_{0.0f};			// Accumulates delta time to orchestrate time-dependent behavior
	float time_to_clap_{1.0f};			// The elapsed time required before the NPC will clap
	
	// Owned textures
	TextureNode hidden_texture_;		// Shown when the NPC is hidden
	TextureNode idle_texture_;			// Shown when the NPC is idle
	TextureNode run_texture_;			// Shown when the NPC is running
};

} // namespace cge

#endif // EXAMPLES_NPC_HPP
