#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include "System.h"
#include "Keyboard.h"
#include "Controller.h"
#include "Settings.h"

class InputSystem : public System {
public:
    InputSystem() = default;
    ~InputSystem() = default;

    void init(Keyboard* keyboard, Controller* controller, Settings* settings);

    void update();

private:
    bool inputDown(InputEvent input);
    bool inputUp(InputEvent input);
    bool inputPressed(InputEvent input);
    bool inputReleased(InputEvent input);

    Keyboard* _keyboard = nullptr;
    Controller* _controller = nullptr;
    Settings* _settings = nullptr;
};

#endif