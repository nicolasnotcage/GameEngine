/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef PLATFORM_SCENE_MANAGER_HPP
#define PLATFORM_SCENE_MANAGER_HPP

#include "platform/scene.hpp"
#include "platform/io_handler.hpp"
#include "platform/core.hpp"

#include <vector>
#include <memory>
#include <iostream>

namespace cge
{


// Singleton class to manage a stack of Scene objects
class SceneManager
{
public:
    
    // Get instance of SceneManager
    static SceneManager* get_instance()
    {
        static SceneManager instance;
        return &instance;
    }

    // Delete copy and move constructor/assignment operators
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager(SceneManager&&) = delete;
    SceneManager& operator=(SceneManager&&) = delete;

    // Initialize scene manager with SDL Info and IO handler
    void init(SDLInfo* sdl_info, IoHandler* io_handler)
    {
        sdl_info_ = sdl_info;
        io_handler_ = io_handler;
    }

    // Push scene to stack and make it active
    void push_scene(Scene* scene)
    {
        // Pause the current scene if there is one
        if (!scene_stack_.empty())
        {
            scene_stack_.back()->on_pause();
        }

        // Initialize and push the new scene
        scene->init(sdl_info_, io_handler_);
        scene->on_enter();
        scene_stack_.push_back(scene);
    }

    // Pop top scene from the stack; returns true if popped, false if stack empty
    bool pop_scene()
    {
        if (scene_stack_.empty())
        {
            return false;
        }

        // Clean up the current scene
        scene_stack_.back()->on_exit();
        scene_stack_.back()->destroy();
        scene_stack_.pop_back();

        // Resume the new top scene if there is one
        if (!scene_stack_.empty())
        {
            scene_stack_.back()->on_resume();
        }

        return true;
    }

    // Replace top scene with new scene; returns true if replaced, false if stack empty
    bool replace_scene(Scene* scene)
    {
        if (scene_stack_.empty())
        {
            push_scene(scene);
            return false;
        }

        // Clean up the current scene
        scene_stack_.back()->on_exit();
        scene_stack_.back()->destroy();
        scene_stack_.pop_back();

        // Initialize and push the new scene
        scene->init(sdl_info_, io_handler_);
        scene->on_enter();
        scene_stack_.push_back(scene);

        return true;
    }

    // Get current active scene; returns nullptr if empty
    Scene* get_current_scene()
    {
        if (scene_stack_.empty())
        {
            return nullptr;
        }
        return scene_stack_.back();
    }

    // Update current scene
    void update(double delta)
    {
        if (!scene_stack_.empty())
        {
            scene_stack_.back()->update(delta);
        }
    }

    // Render current scene
    void render()
    {
        if (!scene_stack_.empty())
        {
            scene_stack_.back()->render();
        }
    }

    // Clean up all scenes and clear the stack
    void clear_all_scenes()
    {
        while (!scene_stack_.empty())
        {
            scene_stack_.back()->on_exit();
            scene_stack_.back()->destroy();
            scene_stack_.pop_back();
        }
    }

private:
    SceneManager() = default;
    ~SceneManager()
    {
        clear_all_scenes();
    }

    std::vector<Scene*> scene_stack_;
    SDLInfo* sdl_info_ = nullptr;
    IoHandler* io_handler_ = nullptr;
};

} // namespace cge

#endif // PLATFORM_SCENE_MANAGER_HPP
