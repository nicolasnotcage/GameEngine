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
    static GameManager *get_instance();

    // Templated game loop that uses T for a scene type
    template <typename T>
    void run_game_loop(T scene)
    {
        time_manager_->update();
        uint64_t current_time = time_manager_->get_current_time();

        int times_updated = 0;
        while(current_time - last_update_time_ > UPDATE_INTERVAL && times_updated < 3)
        {
            // TODO: update_scene(UPDATE_INTERVAL);
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
    GameManager();
    ~GameManager();

private:
    static GameManager *game_manager_;
    TimeManager        *time_manager_;
    static uint64_t     last_update_time_;
    static uint64_t     last_draw_time_;

    void sleep(int milliseconds);
};

} // namespace cge

#endif // PLATFORM_GAME_MANAGER
