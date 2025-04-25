/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/player.hpp"
#include "examples/npc.hpp"
#include "graph/sprite_node.hpp"
#include "graph/transform_node.hpp"
#include "platform/animation.hpp"
#include "platform/audio_engine.hpp"
#include "platform/audio_component.hpp"
#include "platform/io_handler.hpp"
#include "platform/game_action.hpp"
#include "platform/scene_manager.hpp"
#include "system/save_manager.hpp"
#include "system/serializer.hpp"

#include <iostream>
#include <memory>

namespace cge
{

void Player::update(double delta)
{
	Character::update(delta);

	// Player-specific update functionality
}

void Player::init(SceneState& scene_state)
{
	// Scale player
	transform_node_->right_scale(3.0f, 3.0f);

	// Set player position if no save file exists
	if (!SaveManager::get_instance().save_exists()) set_position(1.0f, 0.0f);

	init_textures(scene_state);
	init_animations();
	init_audio();
}

void Player::init_animations()
{
	if (!sprite_node_) return;

	// Create run animation
	Animation run_animation("run", true);
	for(int i = 0; i < 6; i++) { run_animation.add_frame(i, 10); }

	// Create idle animation
	Animation idle_animation("idle", true);
	for(int i = 0; i < 6; i++) { idle_animation.add_frame(i, 10); }

	// Add animations with their respective textures
	sprite_node_->add_animation_with_texture(run_animation, &run_texture_);
	sprite_node_->add_animation_with_texture(idle_animation, &idle_texture_);
	sprite_node_->set_texture(&idle_texture_);
	sprite_node_->play("idle");

	// Set player as player controlled and associate its transform with its sprite
	if (transform_node_) {
		transform_node_->set_player_controlled();
		transform_node_->set_associated_sprite(sprite_node_);
	}
}

void Player::init_textures(SceneState& scene_state)
{
	// White witch (Player) textures
	configure_texture(run_texture_, "images/white_witch/witch_run.png", scene_state, true, 200, 1, 6, 64, 64);
	configure_texture(idle_texture_, "images/white_witch/witch_idle.png", scene_state, true, 200, 1, 6, 64, 64);
}

void Player::destroy_textures()
{
	run_texture_.destroy();
	idle_texture_.destroy();
}

void Player::init_audio()
{
	if (!transform_node_) return;
	
	// Add audio component to player with whistle sound
	auto* audio_component = transform_node_->add_audio_component();
	if (audio_component) {
		audio_component->set_sound("whistle");
	}
}

void Player::whistle()
{
	if (auto* audio = transform_node_->get_audio_component()) 
	{
		audio->play(1.0f);
	}
}

void Player::process_audio_actions(IoHandler* io_handler, std::shared_ptr<NPC> npc)
{
	const GameActionList &actions = io_handler->get_game_actions();
	for(uint8_t i = 0; i < actions.num_actions; i++)
	{
		if (actions.actions[i] == GameAction::PLAYER_WHISTLE) {
			// Play player whistle sound
			whistle();

			// Set flag for NPC to clap after delay if NPC is hidden
			if (npc->is_hidden() && !npc->is_waiting_to_clap()) 
			{
				npc->set_waiting_to_clap(true);
				npc->reset_audio_timer();
			}
			break;
		}
		
		else if (actions.actions[i] == GameAction::TOGGLE_MUSIC) 
		{
			// Toggle music
			AudioEngine::get_instance()->toggle_music();
			break;
		}
	}
}

void Player::handle_input_actions(IoHandler* io_handler)
{
	// Check for pause action to open pause menu
	const GameActionList &actions = io_handler->get_game_actions();
	for (uint8_t i = 0; i < actions.num_actions; i++)
	{
		if (actions.actions[i] == GameAction::TOGGLE_PAUSE)
		{
			// Push the pause menu
			SceneManager::get_instance()->push_scene_by_key("pause_menu");
			return; // Exit early to prevent further updates this frame
		}
		else if (actions.actions[i] == GameAction::SAVE_GAME)
		{
			// Save the game
			SaveManager::get_instance().save_game(SceneManager::get_instance()->get_current_scene());
			
			// Provide feedback to the player (optional)
			std::cout << "Game saved successfully!" << std::endl;
		}
	}
}

bool Player::is_investigating(IoHandler* io_handler) const
{
	const GameActionList& actions = io_handler->get_game_actions();
	for (uint8_t i = 0; i < actions.num_actions; i++)
	{
		if (actions.actions[i] == GameAction::INVESTIGATE)
		{
			return true;
		}
	}
	return false;
}

// Serializable interface implementation
void Player::serialize(Serializer& serializer) const
{
	// Serialize player position
	float x = transform_node_->get_position_x();
	float y = transform_node_->get_position_y();
	
	serializer.write("player_x", x);
	serializer.write("player_y", y);
}

void Player::deserialize(Serializer& serializer)
{
	// Deserialize player position
	float x = 0.0f, y = 0.0f;
	
	if (serializer.read("player_x", x) && serializer.read("player_y", y)) {
		transform_node_->set_position(x, y);
	}
}

} // namespace cge
