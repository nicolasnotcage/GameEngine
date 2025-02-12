#ifndef PLATFORM_IO_HANDLER
#define PLATFORM_IO_HANDLER

#include "event.hpp"

namespace cge
{

// A singleton game manager class
class IoHandler
{
public:
    IoHandler() = default;
    ~IoHandler() = default;

    void update();
    bool quit_requested() const;

private:
    SDLEventInfo curr_events_;
};

} // namespace cge

#endif // PLATFORM_IO_HANDLER
