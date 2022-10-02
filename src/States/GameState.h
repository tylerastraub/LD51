#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "State.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Controller.h"
#include "Level.h"
#include "Timer.h"
#include "DialogueBox.h"
// Systems
#include "RenderSystem.h"
#include "CollisionSystem.h"
#include "PhysicsSystem.h"
#include "InputSystem.h"
#include "CameraSystem.h"
#include "ScriptSystem.h"
#include "DeathSystem.h"

#include <memory>
#include <cstdint>

using Entity = std::uint16_t;

class GameState: public State {
public:
    GameState() = default;
    ~GameState() = default;

    bool init() override;
    void tick(float timescale) override;
    void render() override;
    void initSystems();
    void handleKeyboardInput(SDL_Event e) override {};
    void handleControllerButtonInput(SDL_Event e) override;
    void handleControllerAxisInput(SDL_Event e) override;
    void handleMouseInput(SDL_Event e) override;

private:
    void resetState();
    void respawnEngines();

    std::unique_ptr<Keyboard> _keyboard = nullptr;
    std::unique_ptr<Mouse> _mouse = nullptr;
    std::unique_ptr<Controller> _controller = nullptr;

    Level _level;

    SDL_FPoint _renderOffset = {0.f, 0.f};

    std::shared_ptr<RenderSystem> _renderSystem = nullptr;
    std::shared_ptr<CollisionSystem> _collisionSystem = nullptr;
    std::shared_ptr<PhysicsSystem> _physicsSystem = nullptr;
    std::shared_ptr<InputSystem> _inputSystem = nullptr;
    std::shared_ptr<CameraSystem> _cameraSystem = nullptr;
    std::shared_ptr<ScriptSystem> _scriptSystem = nullptr;
    std::shared_ptr<DeathSystem> _deathSystem = nullptr;

    Entity _player;

    Timer _timer;
    int _deathTimer = 0;

    DialogueBox _dialogueBox;

    strb::vec2 _checkpointPos = {0.f, 0.f};

    std::vector<strb::vec2> _engineSpawnList;

    bool _gameOver = false;
};

#endif