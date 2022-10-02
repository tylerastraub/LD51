#include "State.h"

void State::requestQuit() {
    _isRequestingQuit = true;
}

void State::setGameSize(int w, int h) {
    _gameSize = {(float) w, (float) h};
}

void State::setNextState(State* state) {
    if(_nextState) delete _nextState;
    _nextState = state;
}

void State::setRenderer(SDL_Renderer* renderer) {
    _renderer = renderer;    
}

void State::setRenderScale(int scale) {
    _renderScale = scale;
}

void State::addText(TextSize size, Text* text) {
    _text[size] = text;
}

void State::setAudioPlayer(Audio* audioPlayer) {
    _audioPlayer = audioPlayer;
}

void State::setSettings(Settings* settings) {
    _settings = settings;
}

void State::completeSettingsChange() {
    _settingsChanged = false;
}

strb::vec2 State::getGameSize() {
    return _gameSize;
}

State* State::getNextState() {
    return _nextState;
}

SDL_Renderer* State::getRenderer() {
    return _renderer;
}

int State::getRenderScale() {
    return _renderScale;
}

Text* State::getText(TextSize size) {
    return _text[size];
}

Audio* State::getAudioPlayer() {
    return _audioPlayer;
}

Settings* State::getSettings() {
    return _settings;
}

bool State::settingsChanged() {
    return _settingsChanged;
}

bool State::isRequestingQuit() {
    return _isRequestingQuit;
}