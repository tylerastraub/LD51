#include "Game.h"
#include "SpritesheetRegistry.h"

#include <chrono>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cmath>

Game::Game(const char * windowTitle) : _windowTitle(windowTitle) {}

Game::~Game() {
    if(_currentState != nullptr) delete _currentState;
    if(_nextState != nullptr) delete _nextState;
}

bool Game::init() {
    bool windowCreatedSuccessfully = false;

    // SDL initialization
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << "SDL failed to initialize. SDL_Error: " << SDL_GetError() << std::endl;
    }
    else {
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")) {
            std::cout << "Warning: Nearest pixel sampling not enabled!" << std::endl;
        }
        if(!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1")) {
            std::cout << "Warning: Vsync not enabled!" << std::endl;
        }

        // Window and renderer initialization
        _settings = std::make_unique<Settings>();
        _settings->loadSettings("settings.cfg");
        _window = SDL_CreateWindow(_windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _settings->getVideoWidth(), _settings->getVideoHeight(), _settings->getVideoMode() | SDL_WINDOW_RESIZABLE);
        if(_window == nullptr)
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }
        else {
            _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
            if(_renderer == nullptr) {
                std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
            }
            else {
                SDL_RenderSetLogicalSize(_renderer, GAME_WIDTH, GAME_HEIGHT);
                if(SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND) == -1) {
                    std::cout << "Error: failed to set render draw blend mode to SDL_BLENDMODE_BLEND. SDL_Error: " << SDL_GetError() << std::endl;
                }
                // SDL_Image initialization
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init( imgFlags ) & imgFlags)) {
                    std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
                }
                if(TTF_Init() == -1) {
                    std::cout << "SDL_ttf could not be initialized! SDL_ttf Error: " << TTF_GetError() << std::endl;
                }
                else {
                    // Gamepad initialization
                    SDL_JoystickEventState(SDL_ENABLE);
                    if(SDL_IsGameController(0)) {
                        _controller = SDL_GameControllerOpen(0);
                        if(_controller == NULL) {
                            std::cout << "Error: Unable to open controller!" << std::endl;
                        }
                        else {
                            std::cout << "Controller connected: " << SDL_GameControllerName(_controller) << std::endl;
                            if(SDL_GameControllerAddMappingsFromFile("res/controllermappings.txt") == -1) {
                                std::cout << "Error loading controller mappings! SDL_Error: " << SDL_GetError() << std::endl;
                            }
                        }
                    }
                    else {
                        std::cout << "No controllers connected." << std::endl;
                    }
                    // Resource loading
                    if(!loadResources()) {
                        std::cout << "Could not load resources!" << std::endl;
                    }
                    else {
                        // State initialization
                        _currentState = new MainMenuState();
                        _currentState->setGameSize(GAME_WIDTH, GAME_HEIGHT);
                        _currentState->setRenderer(_renderer);
                        for(auto it : _text) {
                            _currentState->addText(it.first, it.second.get());
                        }
                        _currentState->setAudioPlayer(_audioPlayer.get());
                        _currentState->setSettings(_settings.get());
                        _currentState->init();
                        SDL_ShowCursor(SDL_DISABLE);
                        windowCreatedSuccessfully = true;
                    }
                }
            }
        }
    }
    
    return windowCreatedSuccessfully;
}

bool Game::loadResources() {
    // Text
    std::shared_ptr<Text> tinyText = std::make_shared<Text>(_renderer);
    if(!tinyText->load(_tinyTextFontPath, 8)) {
        std::cout << "Error: Failed to load font '" << _tinyTextFontPath << "'!" << std::endl;
        return false;
    }
    _text[TextSize::TINY] = tinyText;

    std::shared_ptr<Text> smallText = std::make_shared<Text>(_renderer);
    if(!smallText->load(_smallTextFontPath, 14)) {
        std::cout << "Error: Failed to load font '" << _smallTextFontPath << "'!" << std::endl;
        return false;
    }
    _text[TextSize::SMALL] = smallText;

    std::shared_ptr<Text> mediumText = std::make_shared<Text>(_renderer);
    if(!mediumText->load(_mediumTextFontPath, 20)) {
        std::cout << "Error: Failed to load font '" << _mediumTextFontPath << "'!" << std::endl;
        return false;
    }
    _text[TextSize::MEDIUM] = mediumText;
    
    std::shared_ptr<Text> largeText = std::make_shared<Text>(_renderer);
    if(!largeText->load(_largeTextFontPath, 24)) {
        std::cout << "Error: Failed to load font '" << _largeTextFontPath << "'!" << std::endl;
        return false;
    }
    _text[TextSize::LARGE] = largeText;

    // Spritesheets
    std::shared_ptr<Spritesheet> dialogueSpritesheet = std::make_shared<Spritesheet>();
    if(!dialogueSpritesheet->load(_renderer, "res/spritesheet/dialogue_box.png")) return false;
    dialogueSpritesheet->setTileWidth(320);
    dialogueSpritesheet->setTileHeight(32);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::DIALOGUE_BOX, dialogueSpritesheet);
    
    std::shared_ptr<Spritesheet> defaultTileset = std::make_shared<Spritesheet>();
    if(!defaultTileset->load(_renderer, "res/spritesheet/default_tileset.png")) return false;
    defaultTileset->setTileWidth(16);
    defaultTileset->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::DEFAULT_TILESET, defaultTileset);
    
    std::shared_ptr<Spritesheet> playerSpritesheet = std::make_shared<Spritesheet>();
    if(!playerSpritesheet->load(_renderer, "res/spritesheet/player.png")) return false;
    playerSpritesheet->setTileWidth(24);
    playerSpritesheet->setTileHeight(24);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::PLAYER_SPRITESHEET, playerSpritesheet);
    
    std::shared_ptr<Spritesheet> checkpoint = std::make_shared<Spritesheet>();
    if(!checkpoint->load(_renderer, "res/spritesheet/checkpoint.png")) return false;
    checkpoint->setTileWidth(16);
    checkpoint->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::CHECKPOINT, checkpoint);
    
    std::shared_ptr<Spritesheet> weaponPickup = std::make_shared<Spritesheet>();
    if(!weaponPickup->load(_renderer, "res/spritesheet/weapon_pickup.png")) return false;
    weaponPickup->setTileWidth(16);
    weaponPickup->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::WEAPON_PICKUP, weaponPickup);
    
    std::shared_ptr<Spritesheet> jumpPickup = std::make_shared<Spritesheet>();
    if(!jumpPickup->load(_renderer, "res/spritesheet/jump_pickup.png")) return false;
    jumpPickup->setTileWidth(16);
    jumpPickup->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::JUMP_PICKUP, jumpPickup);
    
    std::shared_ptr<Spritesheet> bootsPickup = std::make_shared<Spritesheet>();
    if(!bootsPickup->load(_renderer, "res/spritesheet/boots_pickup.png")) return false;
    bootsPickup->setTileWidth(16);
    bootsPickup->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::BOOTS_PICKUP, bootsPickup);
    
    std::shared_ptr<Spritesheet> walljumpPickup = std::make_shared<Spritesheet>();
    if(!walljumpPickup->load(_renderer, "res/spritesheet/walljump_pickup.png")) return false;
    walljumpPickup->setTileWidth(16);
    walljumpPickup->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::WALLJUMP_PICKUP, walljumpPickup);
    
    std::shared_ptr<Spritesheet> projectile = std::make_shared<Spritesheet>();
    if(!projectile->load(_renderer, "res/spritesheet/projectile.png")) return false;
    projectile->setTileWidth(8);
    projectile->setTileHeight(8);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::PROJECTILE, projectile);
    
    std::shared_ptr<Spritesheet> flag = std::make_shared<Spritesheet>();
    if(!flag->load(_renderer, "res/spritesheet/flag.png")) return false;
    flag->setTileWidth(16);
    flag->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::FLAG, flag);
    
    std::shared_ptr<Spritesheet> engineSpritesheet = std::make_shared<Spritesheet>();
    if(!engineSpritesheet->load(_renderer, "res/spritesheet/engine.png")) return false;
    engineSpritesheet->setTileWidth(16);
    engineSpritesheet->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::ENGINE_SPRITESHEET, engineSpritesheet);
    
    std::shared_ptr<Spritesheet> splashScreen = std::make_shared<Spritesheet>();
    if(!splashScreen->load(_renderer, "res/spritesheet/splash_screen.png")) return false;
    splashScreen->setTileWidth(320);
    splashScreen->setTileHeight(180);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::SPLASH_SCREEN, splashScreen);

    // Audio
    _audioPlayer = std::make_unique<Audio>();
    if(!_audioPlayer->addAudio(AudioSound::CHARACTER_BLIP, "res/audio/character_blip.wav")) return false;
    if(!_audioPlayer->addAudio(AudioSound::CHECKPOINT_ACTIVATED, "res/audio/item_pickup.wav")) return false;
    if(!_audioPlayer->addAudio(AudioSound::CHECKPOINT_RESPAWN, "res/audio/teleport.wav")) return false;
    if(!_audioPlayer->addAudio(AudioSound::JUMP, "res/audio/jump.wav")) return false;
    if(!_audioPlayer->addAudio(AudioSound::WALLJUMP, "res/audio/walljump.wav")) return false;
    if(!_audioPlayer->addAudio(AudioSound::DEAD, "res/audio/dead.wav")) return false;
    if(!_audioPlayer->addAudio(AudioSound::CLOCK_TICK, "res/audio/clock_tick.wav")) return false;
    if(!_audioPlayer->addAudio(AudioSound::SHOOT, "res/audio/shoot.wav")) return false;
    if(!_audioPlayer->addAudio(AudioSound::GOAL, "res/audio/goal.wav")) return false;

    return true;
}

void Game::startGameLoop() {
    SDL_Event e;
    auto startTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds dTime = std::chrono::milliseconds(0); // deltaTime
    Uint32 millisecondCount = 0;
    Uint32 frames = 0;
    float frameWait = 1.f / 60.f;
    float frameRemainder = 0.f;
    while(_exitFlag == false) {
        // Event Handling
        while(SDL_PollEvent(&e) != 0) {
            switch(e.type) {
                case SDL_QUIT:
                    _exitFlag = true;
                    break;
                case SDL_KEYDOWN:
                    _currentState->handleKeyboardInput(e);
                    break;
                case SDL_CONTROLLERBUTTONDOWN:
                case SDL_CONTROLLERBUTTONUP:
                    _currentState->handleControllerButtonInput(e);
                    break;
                case SDL_CONTROLLERAXISMOTION:
                    _currentState->handleControllerAxisInput(e);
                    break;
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    _currentState->handleMouseInput(e);
                    break;
                default:
                    break;
            }
        }

        if(_currentState->getNextState() != nullptr) {
            State* tempState = _currentState->getNextState();
            delete _currentState;
            _currentState = tempState;
            _currentState->setGameSize(GAME_WIDTH, GAME_HEIGHT);
            _currentState->setRenderer(_renderer);
            for(auto it : _text) {
                _currentState->addText(it.first, it.second.get());
            }
            _currentState->setAudioPlayer(_audioPlayer.get());
            _currentState->setSettings(_settings.get());
            _currentState->init();
        }

        // Settings changed
        if(_currentState->settingsChanged()) {
            _currentState->getSettings()->saveSettings();
            _settings->loadSettings("settings.cfg"); // this is a dumb hack since i don't wanna make a copy constructor
            SDL_SetWindowSize(_window, _settings->getVideoWidth(), _settings->getVideoHeight());
            if(_settings->getVideoMode() == SDL_WINDOW_FULLSCREEN) {
                SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
            }
            else {
                SDL_bool windowed = SDL_FALSE;
                if(_settings->getVideoMode() == SDL_WINDOW_SHOWN) windowed = SDL_TRUE;
                SDL_SetWindowBordered(_window, windowed);
                SDL_SetWindowResizable(_window, windowed);
            }
            SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            _currentState->completeSettingsChange();
        }

        dTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
        if(dTime.count() >= (frameWait * 1000.f - frameRemainder)) {
            startTime = std::chrono::high_resolution_clock::now();
            millisecondCount += dTime.count();
            frameRemainder += std::abs(frameWait * 1000.f - std::ceil(frameWait * 1000.f));
            if(frameRemainder > 1.f) frameRemainder = 0.f;
            _currentState->tick(frameWait);
            _currentState->render();
            frames++;

            if(_currentState->isRequestingQuit()) _exitFlag = true;
        }

        if(millisecondCount >= 1000) {
            std::cout << "FPS: " << frames << std::endl;
            frames = 0;
            millisecondCount = 0;
        }
    }

    exit();
}

void Game::exit() {
    SDL_DestroyWindow(_window);
    SDL_DestroyRenderer(_renderer);
    SDL_GameControllerClose(_controller);

    _window = nullptr;
    _renderer = nullptr;
    _controller = nullptr;

    IMG_Quit();
    SDL_Quit();
}