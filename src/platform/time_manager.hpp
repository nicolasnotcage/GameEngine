#ifndef PLATFORM_TIME_MANAGER
#define PLATFORM_TIME_MANAGER

#include <chrono>

namespace cge
{

// A singleton game manager class
class TimeManager
{
public:
    static TimeManager* get_instance()
    { 
        static TimeManager instance;
        return &instance;
    }

    // Delete copy and move constructor/assignment operators
    TimeManager(const TimeManager &) = delete;
    TimeManager &operator=(const TimeManager &) = delete;
    TimeManager(TimeManager &&) = delete;
    TimeManager &operator=(TimeManager &&) = delete;

    void update();
    double get_delta_time() const;
    double get_current_time() const;

private:
    TimeManager();
    ~TimeManager();

    std::chrono::high_resolution_clock  my_clock;
    double                             delta_time_;
    double                              last_time_;
};

} // namespace cge

#endif // PLATFORM_TIME_MANAGER
