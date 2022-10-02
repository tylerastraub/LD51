#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include "System.h"
#include "vec2.h"

class CameraSystem : public System {
public:
    CameraSystem() = default;
    ~CameraSystem() = default;

    void update(float timescale);

    void setGoalCameraOffset(float x, float y);
    void setCurrentCameraOffset(float x, float y);
    void setGameSize(int x, int y);
    void setLevelSize(int x, int y);
    void setMaxSpeed(float speed);
    void setAcceleration(float acceleration);

    strb::vec2 getCurrentCameraOffset();
    bool atXEdge();
    bool atYEdge();

private:
    strb::vec2 _currentCameraOffset = {0.f, 0.f};
    strb::vec2 _goalCameraOffset = {0.f, 0.f};
    
    float _cameraAcceleration = 1.f; // How fast the camera gets up to speed
    float _maxSpeed = 1.f; // How fast the camera can move. Should be equal to player's max speed unless focusing on other object
    strb::vec2 _delta = {0.f, 0.f}; // The cameras current DX and DY

    // The max amount of distance the camera can trail the player (or other main subject).
    float _maxCameraDistance = 64.f;

    strb::vec2 _gameSize = {0, 0}; // The size of the game window in pixels
    strb::vec2 _levelSize = {0, 0}; // The level size in pixels

};

#endif