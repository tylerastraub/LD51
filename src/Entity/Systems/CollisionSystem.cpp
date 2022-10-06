#include "CollisionSystem.h"
#include "EntityRegistry.h"
#include "vec2.h"
#include "CollisionComponent.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "PickupComponent.h"
#include "StateComponent.h"
#include "CheckpointComponent.h"
#include "BootsComponent.h"
#include "HealthComponent.h"
#include "ProjectileComponent.h"
#include "GoalComponent.h"
#include "EdgeCheckComponent.h"
#include "PlayerComponent.h"
#include "EnemyComponent.h"

void CollisionSystem::checkForLevelCollisionsOnXAxis(Level* level, float timescale) {
    if(level == nullptr) return;

    auto ecs = EntityRegistry::getInstance();
    int tileSize = level->getTileSize();
    strb::vec2 topLeftTileCoord, bottomRightTileCoord;
    
    for(auto ent : _entities) {
        auto& collisionComp = ecs->getComponent<CollisionComponent>(ent);
        auto& physics = ecs->getComponent<PhysicsComponent>(ent);
        auto& state = ecs->getComponent<StateComponent>(ent);

        topLeftTileCoord.x = (collisionComp.collisionRect.x - 1) / tileSize;
        topLeftTileCoord.y = collisionComp.collisionRect.y / tileSize;
        bottomRightTileCoord.x = (collisionComp.collisionRect.x + collisionComp.collisionRect.w + 1) / tileSize;
        bottomRightTileCoord.y = (collisionComp.collisionRect.y + collisionComp.collisionRect.h - 1) / tileSize;

        std::priority_queue<TileCoords> tileCollisions;
        if(physics.velocity.x < 0.f) {
            collisionComp.collidingRight = false;
            // Entity is moving left - get all tiles to left
            for(int y = topLeftTileCoord.y; y <= bottomRightTileCoord.y; ++y) {
                if(topLeftTileCoord.x >= 0 && bottomRightTileCoord.x < level->getTilemapWidth() &&
                   y >= 0 && y < level->getTilemapHeight()) {
                    if(level->getTileAt(topLeftTileCoord.x, y).type == TileType::SOLID) {
                        // Since all the tiles we are looking at share the same x coordinates,
                        // we can simply just calculate distance on the y axis to find the
                        // closest tile.
                        int dist = collisionComp.collisionRect.y - (y * tileSize);
                        TileCoords t(topLeftTileCoord.x * tileSize, y * tileSize, std::abs(dist));
                        tileCollisions.emplace(t);
                    }
                }
            }

            // If we have a collision:
            if(!tileCollisions.empty()) {
                if(ecs->hasComponent<ProjectileComponent>(ent)) {
                    ecs->destroyEntity(ent);
                    return;
                }
                auto& transform = ecs->getComponent<TransformComponent>(ent);
                collisionComp.collidingLeft = true;
                TileCoords closestTile = tileCollisions.top();
                // then place entity as close as possible to right of tile
                transform.position.x = closestTile.x + tileSize - collisionComp.collisionRectOffset.x;
                collisionComp.collisionRect.x = transform.position.x + collisionComp.collisionRectOffset.x;
                collisionComp.collisionRect.y = transform.position.y + collisionComp.collisionRectOffset.y;
                physics.velocity.x = 0.f;
                if(physics.touchingGround) state.state = EntityState::IDLE;
            }
            else {
                collisionComp.collidingLeft = false;
            }
        }
        else if(physics.velocity.x > 0.f) {
            collisionComp.collidingLeft = false;
            // Entity is moving right - get all tiles to right
            for(int y = topLeftTileCoord.y; y <= bottomRightTileCoord.y; ++y) {
                if(topLeftTileCoord.x >= 0 && bottomRightTileCoord.x < level->getTilemapWidth() &&
                   y >= 0 && y < level->getTilemapHeight()) {
                    if(level->getTileAt(bottomRightTileCoord.x, y).type == TileType::SOLID) {
                        // Since all the tiles we are looking at share the same x coordinates,
                        // we can simply just calculate distance on the y axis to find the
                        // closest tile.
                        int dist = collisionComp.collisionRect.y - (y * tileSize);
                        TileCoords t(bottomRightTileCoord.x * tileSize, y * tileSize, std::abs(dist));
                        tileCollisions.emplace(t);
                    }
                }
            }

            // If we have a collision:
            if(!tileCollisions.empty()) {
                if(ecs->hasComponent<ProjectileComponent>(ent)) {
                    ecs->destroyEntity(ent);
                    return;
                }
                auto& transform = ecs->getComponent<TransformComponent>(ent);
                collisionComp.collidingRight = true;
                TileCoords closestTile = tileCollisions.top();
                // then place entity as close as possible to left of tile
                transform.position.x = closestTile.x - collisionComp.collisionRect.w - collisionComp.collisionRectOffset.x;
                collisionComp.collisionRect.x = transform.position.x + collisionComp.collisionRectOffset.x;
                collisionComp.collisionRect.y = transform.position.y + collisionComp.collisionRectOffset.y;
                physics.velocity.x = 0.f;
                if(physics.touchingGround) state.state = EntityState::IDLE;
            }
            else {
                collisionComp.collidingRight = false;
            }
        }
        else {
            // DX is 0 so no collisions either way
            collisionComp.collidingLeft = false;
            collisionComp.collidingRight = false;
        }
    }
}

void CollisionSystem::checkForLevelCollisionsOnYAxis(Level* level, float timescale) {
    if(level == nullptr) return;

    auto ecs = EntityRegistry::getInstance();
    int tileSize = level->getTileSize();
    strb::vec2 topLeftTileCoord, bottomRightTileCoord;
    
    for(auto ent : _entities) {
        auto& collisionComp = ecs->getComponent<CollisionComponent>(ent);
        auto& physics = ecs->getComponent<PhysicsComponent>(ent);

        topLeftTileCoord.x = collisionComp.collisionRect.x / tileSize;
        topLeftTileCoord.y = collisionComp.collisionRect.y / tileSize;
        bottomRightTileCoord.x = (collisionComp.collisionRect.x + collisionComp.collisionRect.w - 1) / tileSize;
        int yDiff = 1;
        if(physics.velocity.y * timescale < 1) yDiff = 0;
        bottomRightTileCoord.y = (collisionComp.collisionRect.y + collisionComp.collisionRect.h - yDiff) / tileSize;

        std::priority_queue<TileCoords> tileCollisions;
        std::priority_queue<TileCoords> hazardCollisions;
        if(physics.velocity.y < 0.f) {
            collisionComp.collidingDown = false;
            physics.touchingGround = false;
            // Entity is moving up - get all tiles to top
            for(int x = topLeftTileCoord.x; x <= bottomRightTileCoord.x; ++x) {
                if(topLeftTileCoord.y >= 0 && bottomRightTileCoord.y < level->getTilemapHeight() &&
                   x >= 0 && x < level->getTilemapWidth()) {
                    if(level->getTileAt(x, topLeftTileCoord.y).type == TileType::SOLID) {
                        // Since all the tiles we are looking at share the same y coordinates,
                        // we can simply just calculate distance on the x axis to find the
                        // closest tile.
                        int dist = collisionComp.collisionRect.x - (x * tileSize);
                        TileCoords t(x * tileSize, topLeftTileCoord.y * tileSize, std::abs(dist));
                        tileCollisions.emplace(t);
                    }
                }
            }

            // If we have a collision:
            if(!tileCollisions.empty()) {
                if(ecs->hasComponent<ProjectileComponent>(ent)) {
                    ecs->destroyEntity(ent);
                    return;
                }
                auto& transform = ecs->getComponent<TransformComponent>(ent);
                collisionComp.collidingUp = true;
                TileCoords closestTile = tileCollisions.top();
                // then place entity as close as possible to bottom of tile
                transform.position.y = closestTile.y + tileSize - collisionComp.collisionRectOffset.y;
                collisionComp.collisionRect.x = transform.position.x + collisionComp.collisionRectOffset.x;
                collisionComp.collisionRect.y = transform.position.y + collisionComp.collisionRectOffset.y;
                physics.velocity.y = 0.f;
            }
            else {
                collisionComp.collidingUp = false;
            }
        }
        else if(physics.velocity.y > 0.f) {
            collisionComp.collidingUp = false;
            // Entity is moving down - get all tiles to bottom
            for(int x = topLeftTileCoord.x; x <= bottomRightTileCoord.x; ++x) {
                if(topLeftTileCoord.y >= 0 && bottomRightTileCoord.y < level->getTilemapHeight() &&
                   x >= 0 && x < level->getTilemapWidth()) {
                    if(level->getTileAt(x, bottomRightTileCoord.y).type == TileType::SOLID) {
                        // Since all the tiles we are looking at share the same y coordinates,
                        // we can simply just calculate distance on the x axis to find the
                        // closest tile.
                        int dist = collisionComp.collisionRect.x - (x * tileSize);
                        TileCoords t(x * tileSize, bottomRightTileCoord.y * tileSize, std::abs(dist));
                        tileCollisions.emplace(t);
                    }
                    else if(level->getTileAt(x, bottomRightTileCoord.y).type == TileType::HAZARD) {
                        int dist = collisionComp.collisionRect.x - (x * tileSize);
                        TileCoords t(x * tileSize, bottomRightTileCoord.y * tileSize, std::abs(dist));
                        hazardCollisions.emplace(t);
                    }
                }
            }

            // If we have a collision:
            if(!tileCollisions.empty()) {
                if(ecs->hasComponent<ProjectileComponent>(ent)) {
                    ecs->destroyEntity(ent);
                    return;
                }
                auto& transform = ecs->getComponent<TransformComponent>(ent);
                collisionComp.collidingDown = true;
                physics.touchingGround = true;
                TileCoords closestTile = tileCollisions.top();
                // then place entity as close as possible to top of tile
                transform.position.y = closestTile.y - collisionComp.collisionRect.h - collisionComp.collisionRectOffset.y;
                collisionComp.collisionRect.x = transform.position.x + collisionComp.collisionRectOffset.x;
                collisionComp.collisionRect.y = transform.position.y + collisionComp.collisionRectOffset.y;
                physics.velocity.y = 0.f;
            }
            else if(!hazardCollisions.empty()) {
                if(ecs->hasComponent<BootsComponent>(ent) && physics.velocity.y * timescale < 1.f) {
                    auto& transform = ecs->getComponent<TransformComponent>(ent);
                    collisionComp.collidingDown = true;
                    physics.touchingGround = true;
                    TileCoords closestTile = hazardCollisions.top();
                    transform.position.y = closestTile.y - collisionComp.collisionRect.h - collisionComp.collisionRectOffset.y;
                    collisionComp.collisionRect.x = transform.position.x + collisionComp.collisionRectOffset.x;
                    collisionComp.collisionRect.y = transform.position.y + collisionComp.collisionRectOffset.y;
                    physics.velocity.y = 0.f;
                }
                else {
                    auto& health = ecs->getComponent<HealthComponent>(ent);
                    health.hitpoints = 0;
                    physics.velocity.x = 0;
                    physics.velocity.y = 0;
                }
            }
            else {
                collisionComp.collidingDown = false;
                physics.touchingGround = false;
            }
        }
    }
}

bool CollisionSystem::checkForPlayerAndItemCollisions(Entity player, float timescale, std::string& itemMessage, PickupType& pickupType) {
    auto ecs = EntityRegistry::getInstance();
    auto playerCollision = ecs->getComponent<CollisionComponent>(player);
    for(auto item : ecs->getAllOf<PickupComponent>()) {
        auto& itemCollision = ecs->getComponent<CollisionComponent>(item);
        auto& itemTransform = ecs->getComponent<TransformComponent>(item);
        itemCollision.collisionRect.x = itemTransform.position.x + itemCollision.collisionRectOffset.x;
        itemCollision.collisionRect.y = itemTransform.position.y + itemCollision.collisionRectOffset.y;
        if(SDL_HasIntersection(&playerCollision.collisionRect, &itemCollision.collisionRect)) {
            auto pickup = ecs->getComponent<PickupComponent>(item);
            if(pickup.onPickupScript) pickup.onPickupScript->update(item, timescale, _audioPlayer);
            if(pickup.onPickupMessage.size() > 0) itemMessage = pickup.onPickupMessage;
            pickupType = pickup.pickupType;
            ecs->destroyEntity(item);
            return true;
        }
    }
    return false;
}

bool CollisionSystem::checkForPlayerAndCheckpointCollisions(Entity player, float timescale, Entity& checkpointResult) {
    auto ecs = EntityRegistry::getInstance();
    auto playerCollision = ecs->getComponent<CollisionComponent>(player);
    for(auto checkpoint : ecs->getAllOf<CheckpointComponent>()) {
        auto& checkpointComp = ecs->getComponent<CheckpointComponent>(checkpoint);
        auto checkpointCollision = ecs->getComponent<CollisionComponent>(checkpoint);
        auto& checkpointTransform = ecs->getComponent<TransformComponent>(checkpoint);
        checkpointCollision.collisionRect.x = checkpointTransform.position.x + checkpointCollision.collisionRectOffset.x;
        checkpointCollision.collisionRect.y = checkpointTransform.position.y + checkpointCollision.collisionRectOffset.y;
        if(SDL_HasIntersection(&playerCollision.collisionRect, &checkpointCollision.collisionRect)) {
            checkpointResult = checkpoint;
            return true;
        }
    }
    return false;
}

void CollisionSystem::checkForProjectileAndEnemyCollisions(float timescale) {
    auto ecs = EntityRegistry::getInstance();
    // bad n^2 loop here but such few entities it doesn't matter
    for(auto proj : ecs->getAllOf<ProjectileComponent>()) {
        auto& projCollision = ecs->getComponent<CollisionComponent>(proj);
        auto& projTransform = ecs->getComponent<TransformComponent>(proj);
        projCollision.collisionRect.x = projTransform.position.x + projCollision.collisionRectOffset.x;
        projCollision.collisionRect.y = projTransform.position.y + projCollision.collisionRectOffset.y;
        for(auto ent : ecs->getAllOf<HealthComponent>()) {
            if(ecs->hasComponent<PlayerComponent>(ent)) continue;
            auto entCollision = ecs->getComponent<CollisionComponent>(ent);
            auto& entTransform = ecs->getComponent<TransformComponent>(ent);
            entCollision.collisionRect.x = entTransform.position.x + entCollision.collisionRectOffset.x;
            entCollision.collisionRect.y = entTransform.position.y + entCollision.collisionRectOffset.y;
            if(SDL_HasIntersection(&projCollision.collisionRect, &entCollision.collisionRect)) {
                auto& physics = ecs->getComponent<PhysicsComponent>(ent);
                physics.velocity.x = 0;
                physics.velocity.y = 0;
                auto& health = ecs->getComponent<HealthComponent>(ent);
                auto& projectileComp = ecs->getComponent<ProjectileComponent>(ent);
                health.hitpoints -= projectileComp.damage;
                ecs->destroyEntity(proj);
            }
        }
    }
}

void CollisionSystem::checkForPlayerAndEnemyCollisions(Entity player, float timescale) {
    auto ecs = EntityRegistry::getInstance();
    auto playerCollision = ecs->getComponent<CollisionComponent>(player);
    for(auto ent : ecs->getAllOf<EnemyComponent>()) {
        auto entCollision = ecs->getComponent<CollisionComponent>(ent);
        auto& entTransform = ecs->getComponent<TransformComponent>(ent);
        entCollision.collisionRect.x = entTransform.position.x + entCollision.collisionRectOffset.x;
        entCollision.collisionRect.y = entTransform.position.y + entCollision.collisionRectOffset.y;
        if(SDL_HasIntersection(&playerCollision.collisionRect, &entCollision.collisionRect)) {
            auto& physics = ecs->getComponent<PhysicsComponent>(ent);
            physics.velocity.x = 0;
            physics.velocity.y = 0;
            auto& health = ecs->getComponent<HealthComponent>(player);
            health.hitpoints -= 1;
            return;
        }
    }
}

bool CollisionSystem::checkForPlayerAndGoalCollisions(Entity player, float timescale, Entity& goalResult) {
    auto ecs = EntityRegistry::getInstance();
    auto playerCollision = ecs->getComponent<CollisionComponent>(player);
    for(auto goal : ecs->getAllOf<GoalComponent>()) {
        auto& goalComp = ecs->getComponent<GoalComponent>(goal);
        if(goalComp.activated) continue;
        auto goalCollision = ecs->getComponent<CollisionComponent>(goal);
        auto goalTransform = ecs->getComponent<TransformComponent>(goal);
        goalCollision.collisionRect.x = goalTransform.position.x + goalCollision.collisionRectOffset.x;
        goalCollision.collisionRect.y = goalTransform.position.y + goalCollision.collisionRectOffset.y;
        if(SDL_HasIntersection(&playerCollision.collisionRect, &goalCollision.collisionRect)) {
            goalResult = goal;
            goalComp.onActivatedScript->update(goalResult, timescale, _audioPlayer);
            return true;
        }
    }
    return false;
}


void CollisionSystem::checkIfOnEdge(Level* level) {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        if(!ecs->hasComponent<EdgeCheckComponent>(ent)) continue;
        auto& physics = ecs->getComponent<PhysicsComponent>(ent);
        auto& edgeCheck = ecs->getComponent<EdgeCheckComponent>(ent);
        auto& collision = ecs->getComponent<CollisionComponent>(ent);
        if(physics.offGroundCount > 4) {
            edgeCheck.onLeftEdge = false;
            edgeCheck.onRightEdge = false;
            continue;
        }
        int tileSize = level->getTileSize();
        SDL_Point bottomLeftTileCoord;
        bottomLeftTileCoord.x = (collision.collisionRect.x - 1) / tileSize;
        bottomLeftTileCoord.y = (collision.collisionRect.y + collision.collisionRect.h) / tileSize;
        if(bottomLeftTileCoord.x < 0 || bottomLeftTileCoord.x >= level->getTilemapWidth() ||
           bottomLeftTileCoord.y < 0 || bottomLeftTileCoord.y >= level->getTilemapHeight()) {
            edgeCheck.onLeftEdge = false;
        }
        else {
            edgeCheck.onLeftEdge = level->getTileAt(bottomLeftTileCoord.x, bottomLeftTileCoord.y)
                .type != TileType::SOLID;
        }
        SDL_Point bottomRightTileCoord;
        bottomRightTileCoord.x = (collision.collisionRect.x + collision.collisionRect.w) / tileSize;
        bottomRightTileCoord.y = (collision.collisionRect.y + collision.collisionRect.h) / tileSize;
        if(bottomRightTileCoord.x < 0 || bottomRightTileCoord.x >= level->getTilemapWidth() ||
           bottomRightTileCoord.y < 0 || bottomRightTileCoord.y >= level->getTilemapHeight()) {
            edgeCheck.onRightEdge = false;
        }
        else {
            edgeCheck.onRightEdge = level->getTileAt(bottomRightTileCoord.x, bottomRightTileCoord.y)
                .type != TileType::SOLID;
        }
    }
}