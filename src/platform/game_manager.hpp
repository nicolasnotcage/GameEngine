#ifndef PLATFORM_GAME_MANAGER
#define PLATFORM_GAME_MANAGER

#include "time_manager.hpp"

namespace cge
{
// A singleton game manager class
class GameManager
{
public:
    // Compile-time constants
    static constexpr double NUM_UPDATES_PER_SECOND = 60.0;
    static constexpr double NUM_DRAWS_PER_SECOND = 60.0;
    static constexpr double UPDATE_INTERVAL = 1.0 / NUM_UPDATES_PER_SECOND;
    static constexpr double DRAW_INTERVAL = 1.0 / NUM_DRAWS_PER_SECOND;

    // Static function to get an instance of the game manager object
    static GameManager* get_instance()
    { 
        static GameManager instance;
        return &instance;
    }

    // Delete copy constructor and assignment operator to prevent copying
    GameManager(const GameManager &) = delete;
    GameManager &operator=(const GameManager &) = delete;

    // Templated game loop that can handle arbitrary scene types
    template <typename T>
    void run_game_loop(T& scene)
    {
        time_manager_->update();
        double current_time = time_manager_->get_current_time();
        double delta_time = time_manager_->get_delta_time();

        int times_updated = 0;
        while(current_time - last_update_time_ > UPDATE_INTERVAL && times_updated < 3)
        {
            scene.update(delta_time);
            last_update_time_ += UPDATE_INTERVAL;
            times_updated++;
        }

        if(times_updated == 3) last_update_time_ = current_time;

        if(current_time - last_draw_time_ > DRAW_INTERVAL)
        {
            scene.render();
            last_draw_time_ = current_time;
        }
    }

protected:


private:
    GameManager::GameManager()
    {
        time_manager_ = TimeManager::get_instance();
        last_update_time_ = time_manager_->get_current_time();
        last_draw_time_ = last_update_time_;
    }

    ~GameManager() = default;

    TimeManager        *time_manager_;
    double              last_update_time_{0.0};
    double              last_draw_time_{0.0};

    void sleep(int milliseconds);
};

} // namespace cge

#endif // PLATFORM_GAME_MANAGER
