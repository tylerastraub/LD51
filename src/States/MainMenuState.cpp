#include "MainMenuState.h"
#include "GameState.h"
#include "SpritesheetRegistry.h"

bool MainMenuState::init() {
    _keyboard = std::make_unique<Keyboard>();
    _mouse = std::make_unique<Mouse>(getRenderScale(), getRenderScale());
    _controller = std::make_unique<Controller>();

    return true;
}

void MainMenuState::tick(float timescale) {
    if(_keyboard->isKeyPressed(SDL_SCANCODE_Z)) {
        setNextState(new GameState());
    }

    _keyboard->updateInputs();
    _controller->updateInputs();
}

void MainMenuState::render() {
    SDL_SetRenderDrawColor(getRenderer(), 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(getRenderer());
    
    SpritesheetRegistry::getSpritesheet(SpritesheetID::SPLASH_SCREEN)->render(0, 0, getGameSize().x, getGameSize().y);

    Text* mediumText = getText(TextSize::SMALL);
    mediumText->setString("Arrow keys to move");
    mediumText->render(getGameSize().x / 2 - mediumText->getWidth() / 2 - 20, 100);
    mediumText->setString("Press 'Z' to start");
    mediumText->render(getGameSize().x / 2 - mediumText->getWidth() / 2 - 20, 115);

    SDL_RenderPresent(getRenderer());
}

void MainMenuState::handleControllerButtonInput(SDL_Event e) {
    _controller->updateButtonInputs(e);
}

void MainMenuState::handleControllerAxisInput(SDL_Event e) {
    _controller->updateAxisInputs(e);
}

void MainMenuState::handleMouseInput(SDL_Event e) {
    _mouse->updateInput(e, _renderOffset.x, _renderOffset.y);
}