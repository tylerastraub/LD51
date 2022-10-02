#include "InputSystem.h"
#include "EntityRegistry.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "StateComponent.h"
#include "DirectionComponent.h"
#include "WalljumpComponent.h"
#include "CollisionComponent.h"
#include "WeaponComponent.h"
#include "RenderComponent.h"
#include "Projectile.h"

#include <algorithm>

void InputSystem::init(Keyboard* keyboard, Controller* controller, Settings* settings) {
    _keyboard = keyboard;
    _controller = controller;
    _settings = settings;
}

void InputSystem::update() {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto& inputComponent = ecs->getComponent<InputComponent>(ent);
        auto allowedInputs = inputComponent.allowedInputs;

        auto& physics = ecs->getComponent<PhysicsComponent>(ent);
        auto& state = ecs->getComponent<StateComponent>(ent);
        auto& dir = ecs->getComponent<DirectionComponent>(ent);

        // X inputs
        if(inputDown(InputEvent::LEFT) &&
           std::find(allowedInputs.begin(), allowedInputs.end(), InputEvent::LEFT) != allowedInputs.end()) {
            float acceleration = (physics.touchingGround) ? physics.acceleration.x : physics.airAcceleration.x;
            // check if acceleration is just past max velocity (this allows us to cruise above max speed in certain instances)
            if(physics.velocity.x - acceleration < physics.maxVelocity.x * -1 &&
               physics.velocity.x - acceleration > physics.maxVelocity.x * -1 - acceleration) {
                acceleration = physics.maxVelocity.x - physics.velocity.x * -1;
            } 
            if(physics.velocity.x >= physics.maxVelocity.x * -1) physics.velocity.x -= acceleration;
            if(physics.touchingGround) state.state = EntityState::RUNNING;
            dir.direction = Direction::WEST;
        }
        else if(inputDown(InputEvent::RIGHT) &&
           std::find(allowedInputs.begin(), allowedInputs.end(), InputEvent::RIGHT) != allowedInputs.end()) {
            float acceleration = (physics.touchingGround) ? physics.acceleration.x : physics.airAcceleration.x;
            // check if acceleration is just past max velocity (this allows us to cruise above max speed in certain instances)
            if(physics.velocity.x + acceleration > physics.maxVelocity.x &&
               physics.velocity.x + acceleration < physics.maxVelocity.x + acceleration) {
                acceleration = physics.maxVelocity.x - physics.velocity.x;
            }
            if(physics.velocity.x <= physics.maxVelocity.x) physics.velocity.x += acceleration;
            if(physics.touchingGround) state.state = EntityState::RUNNING;
            dir.direction = Direction::EAST;
        }

        // Y inputs
        if(inputPressed(InputEvent::JUMP) &&
           std::find(allowedInputs.begin(), allowedInputs.end(), InputEvent::JUMP) != allowedInputs.end()) {
            if(physics.offGroundCount < 5) {
                physics.velocity.y = physics.jumpPower * -1.f;
                state.state = EntityState::JUMPING;
                physics.touchingGround = false;
                physics.offGroundCount = 5;
                _audioPlayer->playAudio(ent, AudioSound::JUMP, 1.f);
            }
            else if(ecs->hasComponent<WalljumpComponent>(ent)) {
                auto collision = ecs->getComponent<CollisionComponent>(ent);
                if(collision.collidingLeft || collision.collidingRight) {
                    float coefficient = (collision.collidingLeft) ? 1.f : -1.f;
                    physics.velocity.x += 200.f * coefficient;
                    physics.velocity.y = physics.jumpPower * -1.f;
                    state.state = EntityState::JUMPING;
                    _audioPlayer->playAudio(ent, AudioSound::WALLJUMP, 1.f);
                }
            }
        }
        else if(inputDown(InputEvent::DOWN) &&
           std::find(allowedInputs.begin(), allowedInputs.end(), InputEvent::DOWN) != allowedInputs.end()) {
            std::cout << "down pressed" << std::endl;
        }

        // Other inputs
        if(inputPressed(InputEvent::SHOOT) &&
           std::find(allowedInputs.begin(), allowedInputs.end(), InputEvent::JUMP) != allowedInputs.end()) {
            if(ecs->hasComponent<WeaponComponent>(ent)) {
                auto& weapon = ecs->getComponent<WeaponComponent>(ent);
                if(weapon.timeSinceLastShot >= weapon.shotCooldown) {
                    auto renderComp = ecs->getComponent<RenderComponent>(ent);
                    strb::vec2 spawnPos = {
                        (float) renderComp.renderQuad.x + renderComp.renderQuadOffset.x + renderComp.renderQuad.w / 2,
                        (float) renderComp.renderQuad.y + renderComp.renderQuadOffset.y + renderComp.renderQuad.h / 2
                    };
                    prefab::Projectile::create(spawnPos, dir.direction);
                    weapon.timeSinceLastShot = 0;
                    _audioPlayer->playAudio(ent, AudioSound::SHOOT, 1.f);
                }
            }
        }
    }

    _keyboard->updateInputs();
    _controller->updateInputs();
}

bool InputSystem::inputDown(InputEvent input) {
    if(input == InputEvent::LEFT) {
        return _controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == -1 ||
            _controller->isButtonDown(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_LEFT) ||
            _keyboard->isKeyDown(_settings->getScancode(InputEvent::LEFT));
    }
    else if(input == InputEvent::RIGHT) {
        return _controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == 1 ||
            _controller->isButtonDown(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_RIGHT) ||
            _keyboard->isKeyDown(_settings->getScancode(InputEvent::RIGHT));
    }
    else if(input == InputEvent::UP) {
        return _controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == -1 ||
            _controller->isButtonDown(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_UP) ||
            _keyboard->isKeyDown(_settings->getScancode(InputEvent::UP));
    }
    else if(input == InputEvent::DOWN) {
        return _controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == 1 ||
            _controller->isButtonDown(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_DOWN) ||
            _keyboard->isKeyDown(_settings->getScancode(InputEvent::DOWN));
    }
    else {
        return _controller->isButtonDown(_settings->getButton(input)) || _keyboard->isKeyDown(_settings->getScancode(input));
    }
}

bool InputSystem::inputUp(InputEvent input) {
    if(input == InputEvent::LEFT) {
        return !(_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == -1 ||
            _controller->isButtonUp(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_LEFT)) ||
            _keyboard->isKeyUp(_settings->getScancode(InputEvent::LEFT));
    }
    else if(input == InputEvent::RIGHT) {
        return !(_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == 1 ||
            _controller->isButtonUp(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) ||
            _keyboard->isKeyUp(_settings->getScancode(InputEvent::RIGHT));
    }
    else if(input == InputEvent::UP) {
        return !(_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == -1 ||
            _controller->isButtonUp(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_UP)) ||
            _keyboard->isKeyUp(_settings->getScancode(InputEvent::UP));
    }
    else if(input == InputEvent::DOWN) {
        return !(_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == 1 ||
            _controller->isButtonUp(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_DOWN)) ||
            _keyboard->isKeyUp(_settings->getScancode(InputEvent::DOWN));
    }
    else {
        return _controller->isButtonUp(_settings->getButton(input)) || _keyboard->isKeyUp(_settings->getScancode(input));
    }
}

bool InputSystem::inputPressed(InputEvent input) {
    if(input == InputEvent::LEFT) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == -1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) != -1) ||
            _controller->isButtonPressed(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_LEFT) ||
            _keyboard->isKeyPressed(_settings->getScancode(InputEvent::LEFT));
    }
    else if(input == InputEvent::RIGHT) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == 1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) != 1) ||
            _controller->isButtonPressed(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_RIGHT) ||
            _keyboard->isKeyPressed(_settings->getScancode(InputEvent::RIGHT));
    }
    else if(input == InputEvent::UP) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == -1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) != -1) ||
            _controller->isButtonPressed(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_UP) ||
            _keyboard->isKeyPressed(_settings->getScancode(InputEvent::UP));
    }
    else if(input == InputEvent::DOWN) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == 1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) != 1) ||
            _controller->isButtonPressed(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_DOWN) ||
            _keyboard->isKeyPressed(_settings->getScancode(InputEvent::DOWN));
    }
    else {
        return _controller->isButtonPressed(_settings->getButton(input)) || _keyboard->isKeyPressed(_settings->getScancode(input));
    }
}

bool InputSystem::inputReleased(InputEvent input) {
    if(input == InputEvent::LEFT) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) != -1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == -1) ||
            _controller->isButtonReleased(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_LEFT) ||
            _keyboard->isKeyReleased(_settings->getScancode(InputEvent::LEFT));
    }
    else if(input == InputEvent::RIGHT) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) != 1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == 1) ||
            _controller->isButtonReleased(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_RIGHT) ||
            _keyboard->isKeyReleased(_settings->getScancode(InputEvent::RIGHT));
    }
    else if(input == InputEvent::UP) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) != -1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == -1) ||
            _controller->isButtonReleased(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_UP) ||
            _keyboard->isKeyReleased(_settings->getScancode(InputEvent::UP));
    }
    else if(input == InputEvent::DOWN) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) != 1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == 1) ||
            _controller->isButtonReleased(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_DOWN) ||
            _keyboard->isKeyReleased(_settings->getScancode(InputEvent::DOWN));
    }
    else {
        return _controller->isButtonReleased(_settings->getButton(input)) || _keyboard->isKeyReleased(_settings->getScancode(input));
    }
}