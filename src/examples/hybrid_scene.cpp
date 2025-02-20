///*
//Developed for JHU 605.668: Computer Gaming Engines
//
//This is free and unencumbered software released into the public domain.
//For more information, please refer to <https://unlicense.org>
//*/
//
//#include "examples/hybrid_scene.hpp"
//
//namespace cge
//{
//
//void HybridScene::init(SDLInfo *sdl_info)
//{
//    sdl_info_ = sdl_info;
//    
//    SDL_SetRenderDrawColor(sdl_info->renderer, 64, 80, 64, 255);
//    SDL_SetRenderDrawBlendMode(sdl_info->renderer, SDL_BLENDMODE_BLEND);
//    
//    uint8_t green[3] = {0, 255, 0};
//    uint8_t red[3] = {255, 0, 0};
//   
//    // Static Nodes
//    auto &tex_0 = root_.get_child<0>();
//    tex_0.set_filepath("images/box.png");
//    tex_0.set_blend(true);
//    tex_0.set_blend_alpha(200);
//    tex_0.set_color_mods(green);
//
//    auto &geo_0 = tex_0.get_child<0>();    
//    geo_0.set_top_left(100.0f, 100.0f);
//    geo_0.set_top_right(300.0f, 100.0f);
//    geo_0.set_bottom_left(100.0f, 300.0f);
//    
//    // Dynamic Nodes
//    auto tex_1 = std::make_shared<TextureNode>();
//    tex_1->set_filepath("images/circle.png");
//    tex_1->set_blend(true);
//    tex_1->set_blend_alpha(200);
//    tex_1->set_color_mods(red);
//
//    auto geo_1 = std::make_shared<GeometryNode>();
//    geo_1->set_top_left(400.0f, 100.0f);
//    geo_1->set_top_right(600.0f, 100.0f);
//    geo_1->set_bottom_left(400.0f, 300.0f);
//    
//    root_.add_child(tex_1);
//    tex_1->add_child(geo_1);
//
//    scene_state_.reset();
//    scene_state_.sdl_info = sdl_info_;
//    root_.init(scene_state_);
//}
//
//void HybridScene::destroy()
//{
//    root_.destroy();
//}
//
//void HybridScene::render()
//{
//    scene_state_.reset();
//    scene_state_.sdl_info = sdl_info_;
//    
//    root_.draw(scene_state_);
//}
//
//} // namespace cge
