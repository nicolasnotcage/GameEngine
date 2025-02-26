#ifndef PLATFORM_INPUT_INTERPRETER
#define PLATFORM_INPUT_INTERPRETER

#include "platform/event.hpp"
#include "platform/game_action.hpp"

namespace cge
{

// A singleton game manager class
class InputInterpreter
{
public:
    InputInterpreter() = default;
    ~InputInterpreter() = default;

    // Convert raw inputs into game actions
    GameActionList interpret_events(const SDLEventInfo &events);

private:
};

} // namespace cge

#endif // PLATFORM_INPUT_INTERPRETER
