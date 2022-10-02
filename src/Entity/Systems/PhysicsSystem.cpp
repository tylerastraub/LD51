#include "PhysicsSystem.h"
#include "EntityRegistry.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "CollisionComponent.h"

#include <iostream>
#include <algorithm>

bool PhysicsSystem::updateX(float timescale) {
    bool entityMoved = false;
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto& physics = ecs->getComponent<PhysicsComponent>(ent);
        auto& transform = ecs->getComponent<TransformComponent>(ent);

        transform.lastPosition = transform.position; // always update this since last position is based on tile position previous turn
        if(physics.velocity.x != 0.f) {
            entityMoved = true;
            transform.position.x += physics.velocity.x * timescale;
            float friction = (physics.touchingGround) ? physics.frictionCoefficient : physics.airFrictionCoefficient;
            moveToZero(physics.velocity.x, friction);
            
            if(ecs->hasComponent<CollisionComponent>(ent)) {
                auto& collision = ecs->getComponent<CollisionComponent>(ent);
                collision.collisionRect.x = transform.position.x + collision.collisionRectOffset.x;
            }
        }
    }
    return entityMoved;
}

bool PhysicsSystem::updateY(float timescale) {
    bool entityMoved = false;
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        entityMoved = true;
        auto& physics = ecs->getComponent<PhysicsComponent>(ent);
        auto& transform = ecs->getComponent<TransformComponent>(ent);

        if(physics.touchingGround) {
            physics.offGroundCount = 0;
        }
        else {
            ++physics.offGroundCount;
        }

        physics.velocity.y += physics.gravity;
        if(physics.velocity.y > physics.maxVelocity.y) physics.velocity.y = physics.maxVelocity.y;
        transform.position.y += physics.velocity.y * timescale;

        if(ecs->hasComponent<CollisionComponent>(ent)) {
            auto& collision = ecs->getComponent<CollisionComponent>(ent);
            collision.collisionRect.y = transform.position.y + collision.collisionRectOffset.y;
        }
    }

    return entityMoved;
}

void PhysicsSystem::setLevel(Level level) {
    _level = level;
}

void PhysicsSystem::moveToZero(float &value, float amount) {
    if(value != 0.f) {
        if(value > 0.f) {
            value = (value > amount) ? value - amount : 0.f;
        }
        else {
            value = (std::abs(value) > amount) ? value + amount : 0.f;
        }
    }
}