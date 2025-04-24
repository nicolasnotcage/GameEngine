/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "examples/player.hpp"
#include "examples/npc.hpp"
#include "graph/transform_node.hpp"
#include "platform/audio_engine.hpp"
#include "platform/audio_component.hpp"
#include "platform/io_handler.hpp"
#include "platform/game_action.hpp"
#include "platform/scene_manager.hpp"
#include "system/save_manager.hpp"

#include <iostream>

namespace cge
{

void Player::update(double delta)
{
	Character::update(delta);

	// Player-specific update functionality
}

void Player::whistle()
{
	if (auto* audio = transform_node_->get_audio_component()) 
	{
		audio->play(1.0f);
	}
}

void Player::process_audio_actions(IoHandler* io_handler, NPC* npc)
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

} // namespace cge
