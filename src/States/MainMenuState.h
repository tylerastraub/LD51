#ifndef MAIN_MENU_STATE
#define MAIN_MENU_STATE

#include "State.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Controller.h"

#include <memory>

class MainMenuState: public State {
public:
    MainMenuState() = default;
    ~MainMenuState() = default;

    bool init() override;
    void tick(float timescale) override;
    void render() override;
    void handleKeyboardInput(SDL_Event e) override {};
    void handleControllerButtonInput(SDL_Event e) override;
    void handleControllerAxisInput(SDL_Event e) override;
    void handleMouseInput(SDL_Event e) override;

private:
    std::unique_ptr<Keyboard> _keyboard = nullptr;
    std::unique_ptr<Mouse> _mouse = nullptr;
    std::unique_ptr<Controller> _controller = nullptr;

    strb::vec2 _renderOffset = {0.f, 0.f};
};

#endif