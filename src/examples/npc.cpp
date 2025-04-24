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
	if (hidden_texture_) {
		sprite_node_->add_animation_with_texture(hidden_animation, hidden_texture_);
	}
	
	if (idle_texture_) {
		sprite_node_->add_animation_with_texture(idle_animation, idle_texture_);
		sprite_node_->set_texture(idle_texture_);
		sprite_node_->play("idle");
	}

	// Associate the transform with its sprite
	if (transform_node_) {
		transform_node_->set_associated_sprite(sprite_node_);
	}
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
	if (sprite_node_ && hidden_texture_)
	{
		sprite_node_->set_auto_animation_enabled(false);
		sprite_node_->set_texture(hidden_texture_);
		sprite_node_->play("hidden");
	}

	hidden_ = true;
}

void NPC::show()
{
	if (sprite_node_ && idle_texture_)
	{
		sprite_node_->set_auto_animation_enabled(true);
		sprite_node_->set_texture(idle_texture_);
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

} // namespace cge
