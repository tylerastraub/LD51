#include "CameraSystem.h"

void CameraSystem::update(float timescale) {
    float xOffsetDiff = _goalCameraOffset.x - _currentCameraOffset.x;
    float yOffsetDiff = _goalCameraOffset.y - _currentCameraOffset.y;
    // X Offset
    if(_currentCameraOffset.x < _goalCameraOffset.x) {
        if(std::abs(xOffsetDiff) > _maxCameraDistance) {
            if(std::abs(xOffsetDiff) > _maxCameraDistance + 2) {
                _delta.x = xOffsetDiff / _maxCameraDistance * _maxSpeed * timescale;
            }
            else {
                _delta.x = _maxSpeed * timescale;
            }
        }
        else {
            _delta.x = xOffsetDiff / _maxCameraDistance * _maxSpeed * timescale;
        }
    }
    else {
        if(std::abs(xOffsetDiff) > _maxCameraDistance) {
            if(std::abs(xOffsetDiff) > _maxCameraDistance + 2) {
                _delta.x = xOffsetDiff / _maxCameraDistance * _maxSpeed * timescale;
            }
            else {
                _delta.x = _maxSpeed * timescale * -1;
            }
        }
        else {
            _delta.x = xOffsetDiff / _maxCameraDistance * _maxSpeed * timescale;
        }
    }
    _currentCameraOffset.x += _delta.x;
    if(_currentCameraOffset.x < 0) {
        _currentCameraOffset.x = 0;
        _delta.x = 0;
    }
    else if(_currentCameraOffset.x > _levelSize.x - _gameSize.x) {
        _currentCameraOffset.x = _levelSize.x - _gameSize.x;
        _delta.x = 0;
    }

    // Y Offset
    _delta.y = yOffsetDiff / _maxCameraDistance * _maxSpeed * timescale;
    _currentCameraOffset.y += _delta.y;
    if(_currentCameraOffset.y < 0) {
        _currentCameraOffset.y = 0;
        _delta.y = 0;
    }
    else if(_currentCameraOffset.y > _levelSize.y - _gameSize.y) {
        _currentCameraOffset.y = _levelSize.y - _gameSize.y;
        _delta.y = 0;
    }
}

void CameraSystem::setGoalCameraOffset(float x, float y) {
    _goalCameraOffset = {x, y};
}

void CameraSystem::setCurrentCameraOffset(float x, float y) {
    _currentCameraOffset = {x, y};
}

void CameraSystem::setGameSize(int x, int y) {
    _gameSize = {(float) x, (float) y};
}

void CameraSystem::setLevelSize(int x, int y) {
    _levelSize = {(float) x, (float) y};
}

void CameraSystem::setMaxSpeed(float speed) {
    _maxSpeed = speed;
}

void CameraSystem::setAcceleration(float acceleration) {
    _cameraAcceleration = acceleration;
}

strb::vec2 CameraSystem::getCurrentCameraOffset() {
    return _currentCameraOffset * -1.f;
}

bool CameraSystem::atXEdge() {
    return _goalCameraOffset.x <= 0 ||
        _goalCameraOffset.x >= _levelSize.x - _gameSize.x;
}

bool CameraSystem::atYEdge() {
    return _goalCameraOffset.y <= 0 ||
        _goalCameraOffset.y >= _levelSize.y - _gameSize.y;
}