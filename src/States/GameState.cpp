#include "GameState.h"
#include "RandomGen.h"
#include "SpritesheetRegistry.h"
#include "EntityRegistry.h"
#include "LevelParser.h"
// Components
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "ScriptComponent.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "CollisionComponent.h"
#include "HealthComponent.h"
#include "CheckpointComponent.h"
#include "WalljumpComponent.h"
#include "ProjectileComponent.h"
#include "BootsComponent.h"
#include "EnemyComponent.h"
// Prefabs
#include "Player.h"
#include "Pickup.h"
#include "Checkpoint.h"
#include "Goal.h"
#include "Engine.h"

#include <chrono>

std::mt19937 RandomGen::randEng{(unsigned int) std::chrono::system_clock::now().time_since_epoch().count()};

bool GameState::init() {
    auto ecs = EntityRegistry::getInstance();
    ecs->init();

    _keyboard = std::make_unique<Keyboard>();
    _mouse = std::make_unique<Mouse>(getRenderScale(), getRenderScale());
    _controller = std::make_unique<Controller>();

    // Level
    Tile tl = {TileType::SOLID, {0, 0, 16, 16}}; // top left
    Tile t = {TileType::SOLID, {1, 0, 16, 16}}; // top
    Tile tr = {TileType::SOLID, {2, 0, 16, 16}}; // top right
    Tile l = {TileType::SOLID, {0, 1, 16, 16}}; // left
    Tile c = {TileType::SOLID, {1, 1, 16, 16}}; // center
    Tile r = {TileType::SOLID, {2, 1, 16, 16}}; // right
    Tile bl = {TileType::SOLID, {0, 2, 16, 16}}; // bot left
    Tile b = {TileType::SOLID, {1, 2, 16, 16}}; // bot
    Tile br = {TileType::SOLID, {2, 2, 16, 16}}; // bot right
    Tile s = {TileType::SOLID, {3, 0, 16, 16}}; // single tile
    Tile v = {TileType::HAZARD, {0, 3, 16, 16}}; // spike tile
    Tile o = {TileType::NOVAL, {0, 0, 0, 0}}; // empty tile
    _level.setTileSize(16);
    _level.setTilemap(LevelParser::parseLevel("res/level/main_level.txt"));
    _level.setTileset(SpritesheetRegistry::getSpritesheet(SpritesheetID::DEFAULT_TILESET));

    initSystems();

    // Prefabs
    strb::vec2 playerSpawn = {96, 168};
    _player = prefab::Player::create(playerSpawn);
    // ecs->addComponent<WalljumpComponent>(_player, WalljumpComponent{});
    // ecs->addComponent<BootsComponent>(_player, BootsComponent{});
    
    prefab::Pickup::create({56, 72}, PickupType::WEAPON);
    prefab::Pickup::create({272, 168}, PickupType::JUMP);
    prefab::Pickup::create({1584, 216}, PickupType::WALLJUMP);
    prefab::Pickup::create({1160, 312}, PickupType::BOOTS);

    prefab::Checkpoint::create({96, 176});
    prefab::Checkpoint::create({608, 88});
    prefab::Checkpoint::create({1120, 136});
    prefab::Checkpoint::create({1552, 216});
    prefab::Checkpoint::create({48, 344});
    prefab::Checkpoint::create({656, 328});
    prefab::Checkpoint::create({1080, 312});

    _engineSpawnList = {
        {1436, 48},
        {416, 336}
    };

    prefab::Goal::create({1592, 336});

    // Other
    ecs->addWatcher(_cameraSystem.get(), _player);

    _timer.setTimer(9999);
    _timer.setTimerResetDefault(9999);
    _timer.setAudio(getAudioPlayer());

    _dialogueBox.setIsEnabled(false);
    _dialogueBox.setAudio(getAudioPlayer());
    _dialogueBox.setReadSpeed(ReadSpeed::MEDIUM);
    _dialogueBox.setText(getText(TextSize::TINY));

    respawnEngines();

    return true;
}

void GameState::tick(float timescale) {
    auto ecs = EntityRegistry::getInstance();

    if(_gameOver) {
        if(_keyboard->isKeyReleased(SDL_SCANCODE_ESCAPE)) {
            requestQuit();
        }
        else if(_keyboard->isKeyReleased(SDL_SCANCODE_R)) {
            setNextState(new GameState());
        }
    }
    
    if(_dialogueBox.isEnabled()) {
        if(_keyboard->isKeyPressed(SDL_SCANCODE_Z)) {
            if(_dialogueBox.isTextFullyDisplayed()) {
                _dialogueBox.setIsEnabled(false);
            }
            else {
                _dialogueBox.setTextFullyDisplayed(true);
            }
        }
        _dialogueBox.tick(timescale);
        _keyboard->updateInputs();
        _controller->updateInputs();
        return;
    }

    if(ecs->getComponent<HealthComponent>(_player).hitpoints <= 0) {
        if(_deathTimer == 0) getAudioPlayer()->playAudio(_player, AudioSound::DEAD, 1.f);
        _deathTimer += timescale * 1000.f;
        if(_deathTimer > 1500) resetState();
        return;
    }

    if(!_gameOver) _timer.update(timescale);
    if(_timer.isZero()) {
        resetState();
    }

    _scriptSystem->update(timescale);

    _inputSystem->update();

    _physicsSystem->updateX(timescale);
    _collisionSystem->checkForLevelCollisionsOnXAxis(&_level, timescale);
    _physicsSystem->updateY(timescale);
    _collisionSystem->checkForLevelCollisionsOnYAxis(&_level, timescale);
    _collisionSystem->checkIfOnEdge(&_level);

    _collisionSystem->checkForProjectileAndEnemyCollisions(timescale);

    std::string pickupMessage = "";
    PickupType pickupType;
    if(_collisionSystem->checkForPlayerAndItemCollisions(_player, timescale, pickupMessage, pickupType)) {
        if(pickupMessage.size() > 0) {
            _dialogueBox.setString(pickupMessage);
            _dialogueBox.reset();
            _dialogueBox.setIsEnabled(true);
            if(pickupType == PickupType::BOOTS) {
                _engineSpawnList.push_back({192, 176});
                _engineSpawnList.push_back({952, 120});
                _engineSpawnList.push_back({1440, 336});
                _engineSpawnList.push_back({1408, 176});
                _engineSpawnList.push_back({680, 144});
                _engineSpawnList.push_back({292, 320});
                _engineSpawnList.push_back({848, 304});
                respawnEngines();
            }
        }
    }

    Entity checkpoint;
    if(_collisionSystem->checkForPlayerAndCheckpointCollisions(_player, timescale, checkpoint)) {
        auto transform = ecs->getComponent<TransformComponent>(checkpoint);
        _checkpointPos = {transform.position.x, transform.position.y - 8};
        for(auto oldCheckpoint : ecs->getAllOf<CheckpointComponent>()) {
            if(oldCheckpoint == checkpoint) {
                auto& newCheckpointComp = ecs->getComponent<CheckpointComponent>(checkpoint);
                newCheckpointComp.isActive = true;
                if(!_wasCollidingWithCheckpointLastFrame) {
                    _timer.reset();
                    _wasCollidingWithCheckpointLastFrame = true;
                    // this is not a great way to test but whateva
                    if(!getAudioPlayer()->isPlaying(-1, AudioSound::CHECKPOINT_RESPAWN)) {
                        newCheckpointComp.onActivatedScript->update(checkpoint, timescale, getAudioPlayer());
                    }
                }
                continue;
            }
            auto& oldCheckpointComp = ecs->getComponent<CheckpointComponent>(oldCheckpoint);
            auto& state = ecs->getComponent<StateComponent>(oldCheckpoint);
            oldCheckpointComp.isActive = false;
            state.state = EntityState::IDLE;
        }
    }
    else {
        _wasCollidingWithCheckpointLastFrame = false;
    }
    
    Entity goal;
    if(_collisionSystem->checkForPlayerAndGoalCollisions(_player, timescale, goal)) {
        _gameOver = true;
    }
    
    _collisionSystem->checkForPlayerAndEnemyCollisions(_player, timescale);

    _deathSystem->update(timescale);

    _renderSystem->update(timescale);

    // Camera
    auto& pTransform = ecs->getComponent<TransformComponent>(_player);
    auto& pRender = ecs->getComponent<RenderComponent>(_player);
    _cameraSystem->setGoalCameraOffset(pTransform.position.x + pRender.renderQuadOffset.x + pRender.renderQuad.w / 2 - getGameSize().x / 2,
        pTransform.position.y + pRender.renderQuadOffset.y + pRender.renderQuad.h / 2 - getGameSize().y / 2);
    _cameraSystem->update(timescale);

    float playerXRemainder = pTransform.position.x - (int) pTransform.position.x;
    float playerYRemainder = pTransform.position.y - (int) pTransform.position.y;
    if(_cameraSystem->atXEdge()) playerXRemainder = 0.f;
    if(_cameraSystem->atYEdge()) playerYRemainder = 0.f;
    _renderOffset.x = (int) (_cameraSystem->getCurrentCameraOffset().x + playerXRemainder);
    _renderOffset.y = (int) (_cameraSystem->getCurrentCameraOffset().y + playerYRemainder);
}

void GameState::render() {
    SDL_SetRenderDrawColor(getRenderer(), 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(getRenderer());

    auto ecs = EntityRegistry::getInstance();

    _level.render(_renderOffset.x, _renderOffset.y);

    _renderSystem->render(getRenderer(), _renderOffset.x, _renderOffset.y);

    if(_dialogueBox.isEnabled()) _dialogueBox.render(0, getGameSize().y - 32);

    // render timer
    Text* smallText = getText(TextSize::SMALL);
    smallText->setString(_timer.getTimerAsString());
    int timerXPos = getGameSize().x / 2 - smallText->getWidth() / 2;
    if(_timer.getMostRecentSecond() < 3) {
        smallText->render(timerXPos, 8, 255, 30, 30);
    }
    else {
        smallText->render(timerXPos, 8);
    }

    // render game over
    if(_gameOver) {
        smallText->setString("You won!");
        smallText->render(5, getGameSize().y - 30);
        Text* tinyText = getText(TextSize::TINY);
        tinyText->setString("Press 'R' to restart or 'ESC' to quit.");
        tinyText->render(5, getGameSize().y - 12);
    }

    SDL_RenderPresent(getRenderer());
}

void GameState::initSystems() {
    auto ecs = EntityRegistry::getInstance();
    Signature sig;

    sig.reset();
    _inputSystem = ecs->registerSystem<InputSystem>();
    _inputSystem->init(_keyboard.get(), _controller.get(), getSettings());
    _inputSystem->_audioPlayer = getAudioPlayer();
    sig.set(ecs->getComponentType<InputComponent>(), true);
    sig.set(ecs->getComponentType<PhysicsComponent>(), true);
    ecs->setSystemSignature<InputSystem>(sig);

    sig.reset();
    _physicsSystem = ecs->registerSystem<PhysicsSystem>();
    _physicsSystem->setLevel(_level);
    sig.set(ecs->getComponentType<TransformComponent>(), true);
    sig.set(ecs->getComponentType<PhysicsComponent>(), true);
    ecs->setSystemSignature<PhysicsSystem>(sig);

    sig.reset();
    _renderSystem = ecs->registerSystem<RenderSystem>();
    _renderSystem->setRenderBounds(getGameSize());
    sig.set(ecs->getComponentType<RenderComponent>(), true);
    ecs->setSystemSignature<RenderSystem>(sig);
    
    sig.reset();
    _collisionSystem = ecs->registerSystem<CollisionSystem>();
    _collisionSystem->_audioPlayer = getAudioPlayer();
    sig.set(ecs->getComponentType<CollisionComponent>(), true);
    sig.set(ecs->getComponentType<TransformComponent>(), true);
    sig.set(ecs->getComponentType<PhysicsComponent>(), true);
    ecs->setSystemSignature<CollisionSystem>(sig);
    
    sig.reset();
    _cameraSystem = ecs->registerSystem<CameraSystem>();
    _cameraSystem->setGameSize(getGameSize().x, getGameSize().y);
    _cameraSystem->setLevelSize(_level.getTilemapWidth() * _level.getTileSize(),
        _level.getTilemapHeight() * _level.getTileSize());
    _cameraSystem->setMaxSpeed(300.f);
    ecs->setSystemSignature<CameraSystem>(sig);

    sig.reset();
    _scriptSystem = ecs->registerSystem<ScriptSystem>();
    _scriptSystem->_audioPlayer = getAudioPlayer();
    sig.set(ecs->getComponentType<ScriptComponent>());
    ecs->setSystemSignature<ScriptSystem>(sig);

    sig.reset();
    _deathSystem = ecs->registerSystem<DeathSystem>();
    _deathSystem->_audioPlayer = getAudioPlayer();
    sig.set(ecs->getComponentType<HealthComponent>());
    ecs->setSystemSignature<DeathSystem>(sig);
}

void GameState::handleControllerButtonInput(SDL_Event e) {
    _controller->updateButtonInputs(e);
}

void GameState::handleControllerAxisInput(SDL_Event e) {
    _controller->updateAxisInputs(e);
}

void GameState::handleMouseInput(SDL_Event e) {
    _mouse->updateInput(e, _renderOffset.x, _renderOffset.y);
}

void GameState::resetState() {
    auto ecs = EntityRegistry::getInstance();
    auto& transform = ecs->getComponent<TransformComponent>(_player);
    auto& collision = ecs->getComponent<CollisionComponent>(_player);
    transform.position = _checkpointPos;
    transform.lastPosition = _checkpointPos;
    collision.collisionRect.x = transform.position.x + collision.collisionRectOffset.x;
    collision.collisionRect.y = transform.position.y + collision.collisionRectOffset.y;
    _timer.reset();
    getAudioPlayer()->playAudio(-1, AudioSound::CHECKPOINT_RESPAWN, 0.8f);
    auto& health = ecs->getComponent<HealthComponent>(_player);
    health.hitpoints = 1;
    _deathTimer = 0;
    respawnEngines();
}

void GameState::respawnEngines() {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : ecs->getAllOf<EnemyComponent>()) {
        ecs->destroyEntity(ent);
    }
    for(auto pos : _engineSpawnList) {
        prefab::Engine::create(pos);
    }
}