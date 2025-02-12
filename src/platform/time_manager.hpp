#ifndef PLATFORM_TIME_MANAGER
#define PLATFORM_TIME_MANAGER

#include <chrono>

namespace cge
{

// A singleton game manager class
class TimeManager
{
public:
    TimeManager();
    ~TimeManager();

    void update();
    uint64_t get_delta_time() const;
    uint64_t get_current_time() const;

private:
    std::chrono::high_resolution_clock  my_clock;
    uint64_t                            delta_time_;
    uint64_t                            last_time_;
};

} // namespace cge

#endif // PLATFORM_TIME_MANAGER
