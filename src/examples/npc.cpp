/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/npc.hpp"
#include "graph/transform_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/sprite_node.hpp"
#include "platform/path.hpp"
#include "platform/audio_component.hpp"
#include "system/serializer.hpp"

namespace cge
{

void NPC::update(double delta)
{
	Character::update(delta);

	// Process audio timing for clapping
	process_audio_timing(delta);
}

void NPC::init_animations()
{
	if (!sprite_node_) return;

	// Create walk animation
	Animation run_animation("run", true);
	for(int i = 0; i < 8; i++) { run_animation.add_frame(i, 10); }

	// Create idle animation
	Animation idle_animation("idle", true);
	for(int i = 0; i < 6; i++) { idle_animation.add_frame(i, 10); }

	// Create hidden animation
	Animation hidden_animation("hidden", true);
	for (int i = 0; i < 8; i++) { hidden_animation.add_frame(i, 10); }

	// Add animations with their respective textures
	sprite_node_->add_animation_with_texture(run_animation, &run_texture_);
	sprite_node_->add_animation_with_texture(idle_animation, &idle_texture_);
	sprite_node_->add_animation_with_texture(hidden_animation, &hidden_texture_);
	
	// Set initial texture and animation
	sprite_node_->set_texture(&idle_texture_);
	sprite_node_->play("idle");

	// Associate the transform with its sprite
	if (transform_node_) {
		transform_node_->set_associated_sprite(sprite_node_);
	}
}

void NPC::init_textures(SceneState& scene_state)
{
	// Blue witch (NPC) textures
	configure_texture(run_texture_, "images/blue_witch/B_witch_run.png", scene_state, true, 200, 1, 8, 32, 48);
	configure_texture(idle_texture_, "images/blue_witch/B_witch_idle.png", scene_state, true, 200, 1, 6, 32, 48);
	configure_texture(hidden_texture_, "images/blue_witch/B_witch_transparent.png", scene_state, true, 0, 1, 8, 32, 48);
}

void NPC::destroy_textures()
{
	run_texture_.destroy();
	idle_texture_.destroy();
	hidden_texture_.destroy();
}

void NPC::init_audio()
{
	if (!transform_node_) return;
	
	// Add audio component to NPC with clap sound and 3D settings
	auto* audio_component = transform_node_->add_audio_component();
	if (audio_component) {
		audio_component->set_sound("npc_clap");
		audio_component->set_min_distance(1.0f);
		audio_component->set_max_distance(10.0f);
	}
}

void NPC::hide()
{
	if (sprite_node_)
	{
		sprite_node_->set_auto_animation_enabled(false);
		sprite_node_->set_texture(&hidden_texture_);
		sprite_node_->play("hidden");
	}

	hidden_ = true;
}

void NPC::show()
{
	if (sprite_node_)
	{
		sprite_node_->set_auto_animation_enabled(true);
		sprite_node_->set_texture(&idle_texture_);
		sprite_node_->play("idle");
	}

	hidden_ = false;
}

void NPC::set_path(Path& path)
{
	transform_node_->set_path_controlled(path);
}

bool NPC::is_moving() const
{
	return transform_node_->is_moving();
}

void NPC::teleport_to(float x, float y)
{
	// Create a new path with the teleport location
	Path new_path{};
	new_path.add_point(x, y, 0.0f);
	new_path.set_looping(false);
	
	// Set the path and teleport to location
	transform_node_->set_path_controlled(new_path);
	transform_node_->set_position(x, y);
}

bool NPC::is_at_position(float y) const
{
	return transform_node_->get_position_y() >= y;
}

void NPC::process_audio_timing(double delta)
{
	// Update clap timer if waiting to clap
	if (waiting_to_clap_) 
	{
		audio_timer_ += delta;
		
		// Check if it's time to clap
		if (audio_timer_ >= time_to_clap_) 
		{
			// Clap if hidden
			clap_if_hidden();
			
			// Reset the timer and flag
			waiting_to_clap_ = false;
			audio_timer_ = 0.0f;
		}
	}
}

void NPC::clap_if_hidden()
{
	// Play the clap sound if NPC is hidden
	if (hidden_)
	{
		if (auto* audio = transform_node_->get_audio_component())
		{
			audio->play(1.0f);
		}
	}
}

// Serializable interface implementation
void NPC::serialize(Serializer& serializer) const
{
	// Serialize NPC position
	float x = transform_node_->get_position_x();
	float y = transform_node_->get_position_y();
	
	serializer.write("npc_x", x);
	serializer.write("npc_y", y);
	
	// Serialize NPC state
	serializer.write("npc_hidden", hidden_);
	serializer.write("npc_waiting_for_dialogue", waiting_for_dialogue_);
	serializer.write("npc_waiting_to_clap", waiting_to_clap_);
	serializer.write("npc_audio_timer", audio_timer_);
}

void NPC::deserialize(Serializer& serializer)
{
	// Deserialize NPC position
	float x = 0.0f, y = 0.0f;
	
	if (serializer.read("npc_x", x) && serializer.read("npc_y", y)) {
		transform_node_->set_position(x, y);
	}
	
	// Deserialize NPC state
	serializer.read("npc_hidden", hidden_);
	serializer.read("npc_waiting_for_dialogue", waiting_for_dialogue_);
	serializer.read("npc_waiting_to_clap", waiting_to_clap_);
	serializer.read("npc_audio_timer", audio_timer_);
	
	// Update visibility based on hidden state
	if (hidden_) {
		hide();
	} else {
		show();
	}
}

} // namespace cge
