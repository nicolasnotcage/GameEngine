#ifndef PLATFORM_GAME_MANAGER
#define PLATFORM_GAME_MANAGER

namespace cge
{

// A singleton game manager class
class GameManager
{
public:
    static GameManager *get_instance();

    // Templated game loop that uses T for a scene type
    template <typename T>
    void run_game_loop(T scene)
    {
        bool run_game = true;
        while(run_game)
        {
            auto curr_events = cge::get_current_events();
            for(uint8_t i = 0; i < curr_events.num_events; ++i)
            {
                switch(curr_events.events[i])
                {
                    case cge::EventType::QUIT: // 'esc' or close window
                        run_game = false;
                        break;
                    default: break;
                }
            }

            scene.render();
            sleep(16);
        }
    }

protected:
    GameManager() = default;
    ~GameManager() = default;

private:
    static GameManager *game_manager_;

    void sleep(int milliseconds);
};

} // namespace cge

#endif // PLATFORM_GAME_MANAGER
