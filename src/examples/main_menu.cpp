#include "main_menu.hpp"
#include "graph/root_node.hpp"
#include "graph/camera_node.hpp"
#include "graph/sprite_node.hpp"
#include "graph/transform_node.hpp"
#include "graph/texture_node.hpp"
#include "graph/geometry_node.hpp"

#include "platform/math.hpp"
#include "platform/scene_manager.hpp"
#include "platform/audio_engine.hpp"
#include "system/config_manager.hpp"
#include "system/save_manager.hpp"
#include "system/file_locator.hpp"

namespace cge
{

void MainMenuScene::init(SDLInfo* sdl_info, IoHandler* io_handler)
{
	sdl_info_ = sdl_info;
	io_handler_ = io_handler;

	SDL_SetRenderDrawColor(sdl_info->renderer, 0, 0, 0, 0);
	SDL_SetRenderDrawBlendMode(sdl_info->renderer, SDL_BLENDMODE_BLEND);

	// Reset scene state
	scene_state_.reset();
	scene_state_.sdl_info = sdl_info_;
	scene_state_.io_handler = io_handler_;

	// Initialize textures
	initialize_textures();

	// Configure camera with dimensions adjusted for screen aspect ratio
	auto &camera = root_.get_child<0>();
	float aspect_ratio = static_cast<float>(cge::ConfigManager::get_instance().get_screen_width()) / 
                         static_cast<float>(cge::ConfigManager::get_instance().get_screen_height());
	float camera_height = 15.0f;
	float camera_width = camera_height * aspect_ratio;
	camera.get_camera().set_dimensions(camera_width, camera_height);
	camera.get_camera().set_position(0.0f, 0.0f);
	
	// Disable zooming on menu
	camera.set_zoom_enabled(false);

	// Get references to components
	auto &menu_background_transform = camera.get_child<0>();
	auto &menu_background_sprite = menu_background_transform.get_child<0>();
	
	// Get title reference
	auto &title_transform = camera.get_child<1>();
	auto &title_sprite = title_transform.get_child<0>();
	
	// Get button references
	auto &new_game_button = camera.get_child<2>();
	auto &load_game_button = camera.get_child<3>();
	auto &settings_button = camera.get_child<4>();
	auto &exit_button = camera.get_child<5>();
	
	// Configure background and title
	menu_background_sprite.set_texture(&background_texture_);
	title_sprite.set_texture(&title_texture_);

	// Scale and position the background to fill the screen
	menu_background_transform.right_scale(camera_width, camera_height); // Match camera dimensions
	menu_background_transform.right_translate(0.0f, 0.0f); // Center in the camera view
	
	// Scale and position the title
	title_transform.right_scale(camera_width - 5.0f, 3.0f);
	title_transform.right_translate(0.0f, -1.8f);

	// Configure buttons
	// New Game button
	auto &new_game_transform = new_game_button.get_child<0>();
	auto &new_game_sprite = new_game_transform.get_child<0>();
	
	// Set up the button with references to nodes in the graph
	new_game_button.set_transform_node(&new_game_transform);
	new_game_button.set_sprite_node(&new_game_sprite);
	new_game_button.set_camera_node(&camera);
	
	// Configure the button
	new_game_button.set_position(0.0f, -2.4f);
	new_game_button.set_size(6.0f, 2.0f);
	new_game_button.set_normal_sprite("images/ui/buttons/new_game/new_game_base_button.png");
	new_game_button.set_hover_sprite("images/ui/buttons/new_game/new_game_button_on_hover.png");
	new_game_button.set_pressed_sprite("images/ui/buttons/new_game/new_game_button_clicked.png");
	new_game_button.set_callback([this]() {
		// Create and push the main scene without loading saved state
		Scene* main_scene = SceneManager::get_instance()->create_scene_by_key("main_scene");
		SceneManager::get_instance()->push_scene(main_scene);
	});
	
	// Load Game button
	auto &load_game_transform = load_game_button.get_child<0>();
	auto &load_game_sprite = load_game_transform.get_child<0>();
	
	// Set up the button with references to nodes in the graph
	load_game_button.set_transform_node(&load_game_transform);
	load_game_button.set_sprite_node(&load_game_sprite);
	load_game_button.set_camera_node(&camera);
	
	// Configure the button
	load_game_button.set_position(0.0f, -0.1f);
	load_game_button.set_size(6.0f, 1.8f);
	load_game_button.set_normal_sprite("images/ui/buttons/load_game/load_game_base_button.png");
	load_game_button.set_hover_sprite("images/ui/buttons/load_game/load_game_button_on_hover.png");
	load_game_button.set_pressed_sprite("images/ui/buttons/load_game/load_game_button_clicked.png");
	load_game_button.set_callback([this]() {
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
	});
	
	// Settings button
	auto &settings_transform = settings_button.get_child<0>();
	auto &settings_sprite = settings_transform.get_child<0>();
	
	// Set up the button with references to nodes in the graph
	settings_button.set_transform_node(&settings_transform);
	settings_button.set_sprite_node(&settings_sprite);
	settings_button.set_camera_node(&camera);
	
	// Configure the button
	settings_button.set_position(0.0f, 2.2f);
	settings_button.set_size(6.0f, 2.0f);
	settings_button.set_normal_sprite("images/ui/buttons/settings/settings_base_button.png");
	settings_button.set_hover_sprite("images/ui/buttons/settings/settings_button_on_hover.png");
	settings_button.set_pressed_sprite("images/ui/buttons/settings/settings_button_clicked.png");
	settings_button.set_callback([this]() {
		// TODO: Add logic to open settings menu
		// Not going to hit this stretch goal by due date
	});
	
	// Exit button
	auto &exit_transform = exit_button.get_child<0>();
	auto &exit_sprite = exit_transform.get_child<0>();
	
	// Set up the button with references to nodes in the graph
	exit_button.set_transform_node(&exit_transform);
	exit_button.set_sprite_node(&exit_sprite);
	exit_button.set_camera_node(&camera);
	
	// Configure the button
	exit_button.set_position(0.0f, 4.5f);
	exit_button.set_size(6.0f, 2.0f);
	exit_button.set_normal_sprite("images/ui/buttons/exit/exit_base_button.png");
	exit_button.set_hover_sprite("images/ui/buttons/exit/exit_button_on_hover.png");
	exit_button.set_pressed_sprite("images/ui/buttons/exit/exit_button_clicked.png");
	exit_button.set_callback([this]() {
		// Request quit
		SDL_Event quit_event;
		quit_event.type = SDL_EVENT_QUIT;
		SDL_PushEvent(&quit_event);
	});
	
	// Setup audio
	setup_audio();
	
	// Play theme music at low volume
	AudioEngine::get_instance()->play_sound("theme_music", 0.2f);
	
	// Mute theme music if configuration set
	bool music_enabled = ConfigManager::get_instance().get_music_enabled();
	if (!music_enabled) AudioEngine::get_instance()->get_channel(3)->setMute(true);

	// Initialize root node
	root_.init(scene_state_);
}

void MainMenuScene::setup_audio()
{
	cge::AudioEngine *audio_engine = cge::AudioEngine::get_instance();
	
	// Locate files
	auto theme_sound_info = locate_path_for_filename("audio/theme_music.mp3");
	
	// Load sounds if not already loaded
	if (!audio_engine->get_sound("theme_music")) {
		audio_engine->load_sound(theme_sound_info.path, "theme_music", false, true);
		audio_engine->reserve_channel_for_sound("theme_music", 3);
	}
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
}

void MainMenuScene::update(double delta)
{
	scene_state_.io_handler = io_handler_;
	scene_state_.delta = delta;

	// Update scene graph - UIButton nodes will handle their own state
	root_.update(scene_state_);
	
	// Update FMOD system once per tick
	cge::AudioEngine::get_instance()->update();
}

void MainMenuScene::destroy()
{
	root_.destroy();
	
	// Destroy background texture
	background_texture_.destroy();
	
	// Destroy title texture
	title_texture_.destroy();
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


}
