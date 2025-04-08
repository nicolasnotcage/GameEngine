#include "main_menu.hpp"
#include "graph/root_node.hpp"
#include "graph/camera_node.hpp"
#include "graph/sprite_node.hpp"
#include "graph/transform_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/geometry_node.hpp"

#include "platform/config.hpp"
#include "platform/math.hpp"
#include "platform/scene_manager.hpp"
#include "system/save_manager.hpp"

namespace cge
{

void MainMenuScene::init(SDLInfo* sdl_info, IoHandler* io_handler)
{
	sdl_info_ = sdl_info;
	io_handler_ = io_handler;

	SDL_SetRenderDrawColor(sdl_info->renderer, 255, 255, 255, 255);
	SDL_SetRenderDrawBlendMode(sdl_info->renderer, SDL_BLENDMODE_BLEND);

	// Reset scene state
	scene_state_.reset();
	scene_state_.sdl_info = sdl_info_;
	scene_state_.io_handler = io_handler_;

	// Initialize textures
	initialize_textures();

	// Configure camera
	auto &camera = root_.get_child<0>();
	camera.get_camera().set_dimensions(20.0f, 15.0f);
	camera.get_camera().set_position(0.0f, 0.0f);

	// TODO: Disable zooming on menus

	// Get references to components
	auto &menu_background_transform = camera.get_child<0>();
	auto &menu_background_sprite = menu_background_transform.get_child<0>();
	
	// Get title reference
	auto &title_transform = camera.get_child<1>();
	auto &title_sprite = title_transform.get_child<0>();
	
	// Get button references
	auto &new_game_transform = camera.get_child<2>();
	auto &new_game_sprite = new_game_transform.get_child<0>();
	
	auto &load_game_transform = camera.get_child<3>();
	auto &load_game_sprite = load_game_transform.get_child<0>();
	
	auto &settings_transform = camera.get_child<4>();
	auto &settings_sprite = settings_transform.get_child<0>();
	
	auto &exit_transform = camera.get_child<5>();
	auto &exit_sprite = exit_transform.get_child<0>();

	// Configure sprite textures
	menu_background_sprite.set_texture(&background_texture_);
	title_sprite.set_texture(&title_texture_);
	new_game_sprite.set_texture(&new_game_texture_);
	load_game_sprite.set_texture(&load_game_texture_);
	settings_sprite.set_texture(&settings_texture_);
	exit_sprite.set_texture(&exit_texture_);

	// Scale and position the background to fill the screen
	menu_background_transform.right_scale(20.0f, 15.0f); // Match camera dimensions
	menu_background_transform.right_translate(0.0f, 0.0f); // Center in the camera view
	
	// Scale and position the title
	title_transform.right_scale(20.0f, 3.0f);
	title_transform.right_translate(0.0f, -1.8f);

	// Scale and position the buttons
	// New Game button
	new_game_transform.right_scale(6.0f, 2.0f); 
	new_game_transform.right_translate(0.0f, -1.2f);
	
	// Load Game button
	load_game_transform.right_scale(6.0f, 2.0f); 
	load_game_transform.right_translate(0.0f, 0.1f);
	
	// Settings button
	settings_transform.right_scale(6.0f, 2.0f); 
	settings_transform.right_translate(0.0f, 1.4f);
	
	// Exit button
	exit_transform.right_scale(6.0f, 2.0f); 
	exit_transform.right_translate(0.0f, 2.7f);
	
	// TOOD: Add main menu music

	// Initialize root node
	root_.init(scene_state_);
}

void MainMenuScene::initialize_textures()
{
	// Background
	background_texture_.set_filepath("images/ui/ui_background.png");
	background_texture_.set_blend(true);
	background_texture_.set_blend_alpha(200);
	background_texture_.init(scene_state_);
	
	// Title
	title_texture_.set_filepath("images/ui/title_text.png");
	title_texture_.set_blend(true);
	title_texture_.set_blend_alpha(255); // Full opacity for title
	title_texture_.init(scene_state_);

	// New Game Button - Base
	new_game_texture_.set_filepath("images/ui/buttons/new_game/new_game_base_button.png");
	new_game_texture_.set_blend(true);
	new_game_texture_.set_blend_alpha(200);
	new_game_texture_.init(scene_state_);

	// New Game Button - On Hover
	new_game_hover_texture_.set_filepath("images/ui/buttons/new_game/new_game_button_on_hover.png");
	new_game_hover_texture_.set_blend(true);
	new_game_hover_texture_.set_blend_alpha(200);
	new_game_hover_texture_.init(scene_state_);

	// New Game Button - Clicked
	new_game_clicked_texture_.set_filepath("images/ui/buttons/new_game/new_game_button_clicked.png");
	new_game_clicked_texture_.set_blend(true);
	new_game_clicked_texture_.set_blend_alpha(200);
	new_game_clicked_texture_.init(scene_state_);
	
	// Load Game Button - Base
	load_game_texture_.set_filepath("images/ui/buttons/load_game/load_game_base_button.png");
	load_game_texture_.set_blend(true);
	load_game_texture_.set_blend_alpha(200);
	load_game_texture_.init(scene_state_);

	// Load Game Button - On Hover
	load_game_hover_texture_.set_filepath("images/ui/buttons/load_game/load_game_button_on_hover.png");
	load_game_hover_texture_.set_blend(true);
	load_game_hover_texture_.set_blend_alpha(200);
	load_game_hover_texture_.init(scene_state_);

	// Load Game Button - Clicked
	load_game_clicked_texture_.set_filepath("images/ui/buttons/load_game/load_game_button_clicked.png");
	load_game_clicked_texture_.set_blend(true);
	load_game_clicked_texture_.set_blend_alpha(200);
	load_game_clicked_texture_.init(scene_state_);
	
	// Settings Button - Base
	settings_texture_.set_filepath("images/ui/buttons/settings/settings_base_button.png");
	settings_texture_.set_blend(true);
	settings_texture_.set_blend_alpha(200);
	settings_texture_.init(scene_state_);

	// Settings Button - On Hover
	settings_hover_texture_.set_filepath("images/ui/buttons/settings/settings_button_on_hover.png");
	settings_hover_texture_.set_blend(true);
	settings_hover_texture_.set_blend_alpha(200);
	settings_hover_texture_.init(scene_state_);

	// Settings Button - Clicked
	settings_clicked_texture_.set_filepath("images/ui/buttons/settings/settings_button_clicked.png");
	settings_clicked_texture_.set_blend(true);
	settings_clicked_texture_.set_blend_alpha(200);
	settings_clicked_texture_.init(scene_state_);
	
	// Exit Button - Base
	exit_texture_.set_filepath("images/ui/buttons/exit/exit_base_button.png");
	exit_texture_.set_blend(true);
	exit_texture_.set_blend_alpha(200);
	exit_texture_.init(scene_state_);

	// Exit Button - On Hover
	exit_hover_texture_.set_filepath("images/ui/buttons/exit/exit_button_on_hover.png");
	exit_hover_texture_.set_blend(true);
	exit_hover_texture_.set_blend_alpha(200);
	exit_hover_texture_.init(scene_state_);

	// Exit Button - Clicked
	exit_clicked_texture_.set_filepath("images/ui/buttons/exit/exit_button_clicked.png");
	exit_clicked_texture_.set_blend(true);
	exit_clicked_texture_.set_blend_alpha(200);
	exit_clicked_texture_.init(scene_state_);
}

void MainMenuScene::update(double delta)
{
	scene_state_.io_handler = io_handler_;
	scene_state_.delta = delta;

	// Get mouse position in screen coordinates
	float mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	Vector2 screen_position(static_cast<float>(mouse_x), static_cast<float>(mouse_y));

	// Convert to world coordinates
	auto &camera = root_.get_child<0>();
	Vector2 world_pos = camera.get_camera().screen_to_world(screen_position, cge::SCREEN_WIDTH, cge::SCREEN_HEIGHT);

	// Update button position and dimensions 
	update_button_stats();
	
	// Update button hover state
	update_hovering(world_pos);

	// Check for click on button
	const GameActionList &actions = io_handler_->get_game_actions();
	for (uint8_t i = 0; i < actions.num_actions; i++)
	{
		if (actions.actions[i] == GameAction::MOUSE_BUTTON_LEFT)
		{
			auto& camera = root_.get_child<0>();
			
			// New Game button click
			if (is_new_game_hovered_)
			{
				auto& button_sprite = camera.get_child<2>().get_child<0>();
				button_sprite.set_texture(&new_game_clicked_texture_);
				
				// Create and push the main scene without loading saved state
				Scene* main_scene = SceneManager::get_instance()->create_scene_by_key("main_scene");
				
				// Initialize the scene with default values (no deserialization)
				SceneManager::get_instance()->push_scene(main_scene);
			}
			
			// Load Game button click
			else if (is_load_game_hovered_)
			{
				auto& button_sprite = camera.get_child<3>().get_child<0>();
				button_sprite.set_texture(&load_game_clicked_texture_);
				
				// Check if save exists
				if (SaveManager::get_instance().save_exists())
				{
					// Create the main scene
					Scene* main_scene = SceneManager::get_instance()->create_scene_by_key("main_scene");
					
					// Initialize the scene
					SceneManager::get_instance()->push_scene(main_scene);
					
					// Load the saved game state
					SaveManager::get_instance().load_game(main_scene);
				}
				else
				{
					// TODO: Show a message that no save file exists
					std::cout << "No save file found. Cannot load game.\n";
				}
			}
			
			// Settings button click
			else if (is_settings_hovered_)
			{
				auto& button_sprite = camera.get_child<4>().get_child<0>();
				button_sprite.set_texture(&settings_clicked_texture_);
				// TODO: Add logic to open settings menu
			}
			
			// Exit button click
			else if (is_exit_hovered_)
			{
				auto& button_sprite = camera.get_child<5>().get_child<0>();
				button_sprite.set_texture(&exit_clicked_texture_);
				// Request quit
				SDL_Event quit_event;
				quit_event.type = SDL_EVENT_QUIT;
				SDL_PushEvent(&quit_event);
			}
		}
	}

	// Update scene graph
	root_.update(scene_state_);
}

void MainMenuScene::destroy()
{
	root_.destroy();
	
	// Destroy background texture
	background_texture_.destroy();
	
	// Destroy title texture
	title_texture_.destroy();
	
	// Destroy New Game button textures
	new_game_texture_.destroy();
	new_game_hover_texture_.destroy();
	new_game_clicked_texture_.destroy();
	
	// Destroy Load Game button textures
	load_game_texture_.destroy();
	load_game_hover_texture_.destroy();
	load_game_clicked_texture_.destroy();
	
	// Destroy Settings button textures
	settings_texture_.destroy();
	settings_hover_texture_.destroy();
	settings_clicked_texture_.destroy();
	
	// Destroy Exit button textures
	exit_texture_.destroy();
	exit_hover_texture_.destroy();
	exit_clicked_texture_.destroy();
}

void MainMenuScene::render()
{
	scene_state_.reset();
	scene_state_.sdl_info = sdl_info_;
	scene_state_.io_handler = io_handler_;

	root_.draw(scene_state_);
}

void MainMenuScene::serialize(Serializer& serializer) const
{
	// TODO: Add serialization as needed
}

void MainMenuScene::deserialize(Serializer& serializer)
{
	// TODO: Add deserialization as needed
}

void MainMenuScene::update_button_stats()
{
	auto& camera = root_.get_child<0>();
	
	// Get New Game button position and dimensions
	auto& new_game_button_transform = camera.get_child<2>();
	new_game_button_position[0] = new_game_button_transform.get_position_x();
	new_game_button_position[1] = new_game_button_transform.get_position_y();
	new_game_button_scale[0] = new_game_button_transform.get_scale_x();
	new_game_button_scale[1] = new_game_button_transform.get_scale_y();
	
	// Get Load Game button position and dimensions
	auto& load_game_button_transform = camera.get_child<3>();
	load_game_button_position[0] = load_game_button_transform.get_position_x();
	load_game_button_position[1] = load_game_button_transform.get_position_y();
	load_game_button_scale[0] = load_game_button_transform.get_scale_x();
	load_game_button_scale[1] = load_game_button_transform.get_scale_y();
	
	// Get Settings button position and dimensions
	auto& settings_button_transform = camera.get_child<4>();
	settings_button_position[0] = settings_button_transform.get_position_x();
	settings_button_position[1] = settings_button_transform.get_position_y();
	settings_button_scale[0] = settings_button_transform.get_scale_x();
	settings_button_scale[1] = settings_button_transform.get_scale_y();
	
	// Get Exit button position and dimensions
	auto& exit_button_transform = camera.get_child<5>();
	exit_button_position[0] = exit_button_transform.get_position_x();
	exit_button_position[1] = exit_button_transform.get_position_y();
	exit_button_scale[0] = exit_button_transform.get_scale_x();
	exit_button_scale[1] = exit_button_transform.get_scale_y();
}

// Updates hovering visuals given mouse position in world coordinates
void MainMenuScene::update_hovering(Vector2 mouse_position)
{
	auto& camera = root_.get_child<0>();
	
	// Check New Game button
	{
		float button_x = new_game_button_position[0];
		float button_y = new_game_button_position[1];
		float button_width = new_game_button_scale[0] * 0.8f; // Use 80% of button width for hit detection
		float button_height = new_game_button_scale[1] * 0.8f; // Use 80% of button height for hit detection

		bool is_hovering = (mouse_position.x >= button_x - button_width / 2 &&
							mouse_position.x <= button_x + button_width / 2 &&
							mouse_position.y >= button_y - button_height / 2 &&
							mouse_position.y <= button_y + button_height / 2);

		if (is_hovering && !is_new_game_hovered_)
		{
			auto &button_sprite = camera.get_child<2>().get_child<0>();
			button_sprite.set_texture(&new_game_hover_texture_);
			is_new_game_hovered_ = true;
		}
		else if (!is_hovering && is_new_game_hovered_)
		{
			auto &button_sprite = camera.get_child<2>().get_child<0>();
			button_sprite.set_texture(&new_game_texture_);
			is_new_game_hovered_ = false;
		}
	}
	
	// Check Load Game button
	{
		float button_x = load_game_button_position[0];
		float button_y = load_game_button_position[1];
		float button_width = load_game_button_scale[0] * 0.8f;
		float button_height = load_game_button_scale[1] * 0.8f;

		bool is_hovering = (mouse_position.x >= button_x - button_width / 2 &&
							mouse_position.x <= button_x + button_width / 2 &&
							mouse_position.y >= button_y - button_height / 2 &&
							mouse_position.y <= button_y + button_height / 2);

		if (is_hovering && !is_load_game_hovered_)
		{
			auto &button_sprite = camera.get_child<3>().get_child<0>();
			button_sprite.set_texture(&load_game_hover_texture_);
			is_load_game_hovered_ = true;
		}
		else if (!is_hovering && is_load_game_hovered_)
		{
			auto &button_sprite = camera.get_child<3>().get_child<0>();
			button_sprite.set_texture(&load_game_texture_);
			is_load_game_hovered_ = false;
		}
	}
	
	// Check Settings button
	{
		float button_x = settings_button_position[0];
		float button_y = settings_button_position[1];
		float button_width = settings_button_scale[0] * 0.8f;
		float button_height = settings_button_scale[1] * 0.8f;

		bool is_hovering = (mouse_position.x >= button_x - button_width / 2 &&
							mouse_position.x <= button_x + button_width / 2 &&
							mouse_position.y >= button_y - button_height / 2 &&
							mouse_position.y <= button_y + button_height / 2);

		if (is_hovering && !is_settings_hovered_)
		{
			auto &button_sprite = camera.get_child<4>().get_child<0>();
			button_sprite.set_texture(&settings_hover_texture_);
			is_settings_hovered_ = true;
		}
		else if (!is_hovering && is_settings_hovered_)
		{
			auto &button_sprite = camera.get_child<4>().get_child<0>();
			button_sprite.set_texture(&settings_texture_);
			is_settings_hovered_ = false;
		}
	}
	
	// Check Exit button
	{
		float button_x = exit_button_position[0];
		float button_y = exit_button_position[1];
		float button_width = exit_button_scale[0] * 0.8f;
		float button_height = exit_button_scale[1] * 0.8f;

		bool is_hovering = (mouse_position.x >= button_x - button_width / 2 &&
							mouse_position.x <= button_x + button_width / 2 &&
							mouse_position.y >= button_y - button_height / 2 &&
							mouse_position.y <= button_y + button_height / 2);

		if (is_hovering && !is_exit_hovered_)
		{
			auto &button_sprite = camera.get_child<5>().get_child<0>();
			button_sprite.set_texture(&exit_hover_texture_);
			is_exit_hovered_ = true;
		}
		else if (!is_hovering && is_exit_hovered_)
		{
			auto &button_sprite = camera.get_child<5>().get_child<0>();
			button_sprite.set_texture(&exit_texture_);
			is_exit_hovered_ = false;
		}
	}
}

}
